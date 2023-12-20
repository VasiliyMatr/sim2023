#ifndef INCL_SIM_ELF_LOAD_HPP
#define INCL_SIM_ELF_LOAD_HPP

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <gelf.h>
#include <libelf.h>
#include <unistd.h>

#include <fcntl.h>
#include <sysexits.h>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim::elf {

class ElfLoader final {
    using PPN = memory::PPN;
    using VPN = memory::VPN;

    using MMUMode = csr::SATP64::MODEValue;

    static constexpr VPN DEFAULT_STACK_BASE = 0x10000000;
    static constexpr VPN DEFAULT_STACK_SIZE = 0x1000;

    static constexpr PPN DEFAULT_TABLE_REGION_SIZE = 0x10;

    memory::PhysMemory &m_pm;

    VPN m_stack_base = DEFAULT_STACK_BASE;
    VPN m_stack_size = DEFAULT_STACK_SIZE;

    MMUMode m_mmu_mode = MMUMode::SV39;

    PPN m_table_region_begin = 0;
    PPN m_table_region_size = DEFAULT_TABLE_REGION_SIZE;
    PPN m_table_region_end = m_table_region_begin + m_table_region_size;

    memory::SimpleMemoryMapper m_mapper{m_pm, m_mmu_mode, m_table_region_begin,
                                        m_table_region_end};

    // Next ppn to map in virtual addrs mode
    PPN m_next_map_ppn = m_table_region_end;

    std::unordered_map<VPN, PPN> m_mapping{};

    NODISCARD auto mapPage(VPN page_vpn) {
        if (m_mmu_mode == MMUMode::BARE) {
            // Map RAM page with same addr
            SIM_ASSERT(m_pm.addRAMPage(page_vpn * memory::PAGE_SIZE));
            return SimStatus::OK;
        }

        // Allocate new RAM page
        PPN page_ppn = m_next_map_ppn++;
        SIM_ASSERT(m_pm.addRAMPage(page_ppn * memory::PAGE_SIZE));

        // Update mapping info
        m_mapping.insert({page_vpn, page_ppn});

        // Add arch mapping
        using Flags = memory::PTEFlags;
        Flags flags{Flags::U_MASK | Flags::R_MASK | Flags::W_MASK |
                    Flags::X_MASK};

        return m_mapper.map({flags, page_vpn, page_ppn});
    }

  public:
    ElfLoader(memory::PhysMemory &pm) : m_pm(pm) {}

    struct MapStackRes final {
        SimStatus status = SimStatus::OK;
        RegValue start_sp = 0;
    };

    NODISCARD MapStackRes mapStack() {
        auto curr_vpn = m_stack_base - m_stack_size;

        for (; curr_vpn != m_stack_base; ++curr_vpn) {
            auto status = mapPage(curr_vpn);
            if (status != SimStatus::OK) {
                return {status, 0};
            }
        }

        return {SimStatus::OK, m_stack_base * memory::PAGE_SIZE};
    }

    struct LoadElfRes final {
        SimStatus status = SimStatus::OK;
        RegValue start_pc = 0;
    };

    LoadElfRes loadElf(const char *elf_name);
};

} // namespace sim::elf

#endif // INCL_SIM_ELF_LOAD_HPP
