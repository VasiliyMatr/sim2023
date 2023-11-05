#include "sim/common.hpp"
#include "sim/csr/value.gen.hpp"
#include <sim/memory.hpp>
#include <type_traits>

namespace sim::memory {

namespace {

using Mode = csr::SATP64::MODEValue;

using PTE = uint64_t;
constexpr size_t PTE_SIZE = sizeof(PTE);

// Svnapot and Svpbmt extensions are not supported
constexpr PTE PTE_RESERWED_MASK = (PTE{0x3FF} << 54) | (PTE{3} << 8);

constexpr bit::BitSize VPN_BIT_STEP = 9;
constexpr bit::BitSize PPN_BIT_STEP = 9;
constexpr bit::BitSize PAGE_BIT_SIZE = 12;

constexpr size_t PAGE_SIZE = 1 << PAGE_BIT_SIZE;

// Mode to number of translation levels
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

class PTEFlags final {
    uint8_t m_flags = 0;

  public:
    PTEFlags(uint8_t flags = 0) : m_flags(flags) {}

    NODISCARD bool v() const noexcept { return m_flags & 1 << 0; }
    NODISCARD bool r() const noexcept { return m_flags & 1 << 1; }
    NODISCARD bool w() const noexcept { return m_flags & 1 << 2; }
    NODISCARD bool x() const noexcept { return m_flags & 1 << 3; }
    NODISCARD bool u() const noexcept { return m_flags & 1 << 4; }
    NODISCARD bool g() const noexcept { return m_flags & 1 << 5; }
    NODISCARD bool a() const noexcept { return m_flags & 1 << 6; }
    NODISCARD bool d() const noexcept { return m_flags & 1 << 7; }
};

NODISCARD bool checkFlags(MMU64::AccessType access_type, PrivLevel priv_level,
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

NODISCARD PhysAddr calcPA(PTE pte, VirtAddr va, size_t i) noexcept {
    static constexpr size_t SV57_LEVELS = 5;
    SIM_ASSERT(i < SV57_LEVELS);

    static constexpr bit::BitIdx PPN_LO = 10;
    static constexpr bit::BitIdx PPN_HI = 53;

    bit::BitIdx used_ppn_lo = PPN_LO + i * PPN_BIT_STEP;

    return bit::maskBits<PhysAddr>(pte, PPN_HI, used_ppn_lo) +
           bit::getBitField<PhysAddr>(va, used_ppn_lo - 1, 0);
}

} // namespace

NODISCARD MMU64::Result MMU64::translate(VirtAddr va, AccessType access_type,
                                         PrivLevel priv_level) noexcept {
    MMU64::Result PAGE_FAULT_RES = {Status::PAGE_FAULT, 0};
    MMU64::Result ACCESS_FAULT_RES = {Status::ACCESS_FAULT, 0};

    auto mode = m_satp64.getMODE();

    if (mode == Mode::BARE) {
        return {Status::OK, va};
    }

    size_t levels = modeToLevels(mode);
    size_t i = levels - 1;

    PTE pte = 0;

    PTEFlags flags{};

    // Page table walk
    for (PhysAddr page_table_pa = m_satp64.getPPN() * PAGE_SIZE;; --i) {
        auto vpn = getVPN(va, i);
        PhysAddr pte_pa = page_table_pa + vpn * PTE_SIZE;

        auto status = m_phys_memory.read(pte_pa, pte);
        if (status != PhysMemory::AccessStatus::OK) {
            return ACCESS_FAULT_RES;
        }

        flags = static_cast<uint8_t>(pte);

        // Check pte validity
        if (flags.v() || (!flags.r() && flags.w())) {
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

        page_table_pa = bit::getBitField<PhysAddr, 53, 10>(pte) * PAGE_SIZE;
    }

    // Check if the access is allowed
    if (!checkFlags(access_type, priv_level, flags, m_mstatus64.getMXR(),
                    m_mstatus64.getSUM())) {
        return PAGE_FAULT_RES;
    }

    // Check superpage align
    if (i > 0) {
        static constexpr bit::BitIdx lo = 10;

        if (bit::getBitField<PTE>(pte, PPN_BIT_STEP * i - 1 + lo, lo)) {
            return PAGE_FAULT_RES;
        }
    }

    // A and D bits are set to 1 by default
    SIM_ASSERT(flags.d() && flags.a());

    // The translation is successful
    return {Status::OK, calcPA(pte, va, i)};
}

} // namespace sim::memory
