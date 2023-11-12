#include "sim/common.hpp"
#include "sim/memory/pte.hpp"
#include <sim/memory.hpp>

namespace sim::memory {

// MMU64::translate helper code
namespace {

using Mode = csr::SATP64::MODEValue;
using AccessType = MMU64::AccessType;

// VPN subfields step
constexpr bit::BitSize VPN_BIT_STEP = 9;
// PPN subfields step
constexpr bit::BitSize PPN_BIT_STEP = 9;

// Mode to translation levels number
NODISCARD size_t modeToLevels(Mode mode) noexcept {
    static constexpr size_t SV39_LEVELS = 3;
    static constexpr size_t SV48_LEVELS = 4;
    static constexpr size_t SV57_LEVELS = 5;

    switch (mode) {
    case Mode::SV39:
        return SV39_LEVELS;
    case Mode::SV48:
        return SV48_LEVELS;
    case Mode::SV57:
        return SV57_LEVELS;

    default:
        SIM_ASSERT(0);
    }

    SIM_ASSERT(0);
}

// Get va.VPN[i]
NODISCARD size_t getVPN(VirtAddr va, size_t i) noexcept {
    static constexpr size_t SV57_LEVELS = 5;
    SIM_ASSERT(i < SV57_LEVELS);

    bit::BitIdx lo = PAGE_BIT_SIZE + i * VPN_BIT_STEP;
    bit::BitIdx hi = lo + VPN_BIT_STEP;

    return bit::getBitField<size_t>(va, hi, lo);
}

// Check leaf PTE flags
NODISCARD bool checkLeafFlags(PrivLevel priv_level, AccessType access_type,
                              PTEFlags flags, bool mxr, bool sum) noexcept {
    bool is_user = priv_level == PrivLevel::USER;
    bool user_ok = flags.u() && is_user;
    bool super_rw_ok = !is_user && (!flags.u() || sum);
    bool super_x_ok = !is_user && !flags.u();

    switch (access_type) {
    case MMU64::AccessType::READ: {
        bool is_readable = flags.r() || (flags.x() && mxr);

        return is_readable && (user_ok || super_rw_ok);
    }

    case MMU64::AccessType::WRITE:
        return flags.w() && (user_ok || super_rw_ok);

    case MMU64::AccessType::FETCH:
        return flags.x() && (user_ok || super_x_ok);

    default:
        SIM_ASSERT(0);
    }

    SIM_ASSERT(0);
}

// Calculate resulting PhysAddr
NODISCARD PhysAddr calcPhysAddr(PTE pte, VirtAddr va, size_t i) noexcept {
    static constexpr size_t SV57_LEVELS = 5;
    SIM_ASSERT(i < SV57_LEVELS);

    bit::BitIdx used_ppn_lo = PTE_PPN_LO + i * PPN_BIT_STEP;

    return bit::maskBits<PhysAddr>(pte, PTE_PPN_HI, used_ppn_lo) +
           bit::getBitField<PhysAddr>(va, used_ppn_lo - 1, 0);
}

} // namespace

NODISCARD MMU64::Result MMU64::translate(PrivLevel priv_level,
                                         AccessType access_type,
                                         VirtAddr va) noexcept {
    static constexpr MMU64::Result PAGE_FAULT_RES = {Status::PAGE_FAULT, 0};
    static constexpr MMU64::Result ACCESS_FAULT_RES = {Status::ACCESS_FAULT, 0};

    auto mode = m_satp64.getMODE();

    if (mode == Mode::BARE) {
        return {Status::OK, va};
    }

    size_t i = modeToLevels(mode) - 1;

    PTE pte = 0;
    PTEFlags flags{};

    // Page table walk
    for (PhysAddr table_ppn = m_satp64.getPPN();; --i) {
        // Read next PTE
        PhysAddr pte_pa = table_ppn * PAGE_SIZE + getVPN(va, i) * sizeof(PTE);

        auto status = m_phys_memory.read(pte_pa, pte);
        if (status != PhysMemory::AccessStatus::OK) {
            return ACCESS_FAULT_RES;
        }

        flags = pte;

        // Check PTE validity
        if (flags.v() || (!flags.r() && flags.w()) || (pte & PTE_RESERWED_MASK)) {
            return PAGE_FAULT_RES;
        }

        // Leaf PTE found
        if (flags.r() || flags.x()) {
            break;
        }

        // Leaf PTE not found
        if (i == 0) {
            return PAGE_FAULT_RES;
        }

        table_ppn = bit::getBitField<PhysAddr, PTE_PPN_HI, PTE_PPN_LO>(pte);
    }

    // Check if the access is allowed
    if (!checkLeafFlags(priv_level, access_type, flags, m_mstatus64.getMXR(),
                        m_mstatus64.getSUM())) {
        return PAGE_FAULT_RES;
    }

    // Check superpage alignment
    if (i > 0) {
        static constexpr bit::BitIdx lo = 10;

        if (bit::getBitField<PTE>(pte, PPN_BIT_STEP * i - 1 + lo, lo)) {
            return PAGE_FAULT_RES;
        }
    }

    // A and D bits are set to 1 by default
    SIM_ASSERT(flags.d() && flags.a());

    // The translation is successful
    return {Status::OK, calcPhysAddr(pte, va, i)};
}

NODISCARD SimpleMemoryMapper::MapStatus
SimpleMemoryMapper::map(MemoryMapping mapping) noexcept {
    VirtAddr va = mapping.vpn() * PAGE_SIZE;

    size_t i = modeToLevels(m_mode) - 1;

    // Write mapping PTEs
    for (PhysAddr table_ppn = m_table_region_begin;; --i) {
        // Read next PTE
        PhysAddr pte_pa = table_ppn * PAGE_SIZE + getVPN(va, i) * sizeof(PTE);

        PTE pte = 0;
        auto status = m_phys_memory.read(pte_pa, pte);
        if (status != PhysMemory::AccessStatus::OK) {
            return MapStatus::PHYS_MEMORY_ERROR;
        }

        // Read PTE is not valid => add new PTE
        if (!PTEFlags(pte).v()) {
            if (i != 0) {
                // Add new page table
                if (m_curr_table == m_table_region_end) {
                    return MapStatus::TABLE_REGION_END;
                }

                PPN new_table = m_curr_table++;
                pte = createPTE(new_table, PTEFlags::FlagsMask::V_MASK);
            } else {
                // Add leaf PTE
                pte = createPTE(mapping.ppn(), mapping.flags());
            }

            // Write PTE
            auto status = m_phys_memory.write(pte_pa, pte);
            if (status != PhysMemory::AccessStatus::OK) {
                return MapStatus::PHYS_MEMORY_ERROR;
            }

            // Mapping created
            if (i == 0) {
                break;
            }
        }

        // Leaf PTE is already set
        if (i == 0) {
            return MapStatus::ALREADY_MAPPED;
        }

        table_ppn = bit::getBitField<PhysAddr, PTE_PPN_HI, PTE_PPN_LO>(pte);
    }

    return MapStatus::OK;
}

} // namespace sim::memory
