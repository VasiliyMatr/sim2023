#ifndef INCL_MEMORY_SIMPLE_MAPPER_HPP
#define INCL_MEMORY_SIMPLE_MAPPER_HPP

#include <sim/csr/idx.gen.hpp>
#include <sim/csr/value.gen.hpp>

#include <sim/memory/phys_memory.hpp>
#include <sim/memory/pte.hpp>

namespace sim::memory {

// Creates translation tables for simple virtual memory mappings
struct SimpleMemoryMapper final {
    using Mode = csr::SATP64::MODEValue;

    enum class MapStatus {
        OK,
        // Error on physical memory access
        PHYS_MEMORY_ERROR,
        // Duplicated mapping is detected
        ALREADY_MAPPED,
        // Table region end is reached
        TABLE_REGION_END,
        // Mapping for table region page is detected
        MAPPING_WITHIN_TABLE_REGION
    };

    // Virtual memory mapping.
    // A and D flags are set by default
    class MemoryMapping final {
        static constexpr uint8_t DEFAULT_FLAGS =
            PTEFlags::V_MASK & PTEFlags::A_MASK & PTEFlags::D_MASK;

        PTEFlags m_flags = 0;
        VPN m_vpn = 0;
        PPN m_ppn = 0;

      public:
        constexpr MemoryMapping(PTEFlags flags, VPN vpn, PPN ppn)
            : m_flags(flags.raw() & DEFAULT_FLAGS), m_vpn(vpn), m_ppn(ppn) {}

        NODISCARD constexpr auto flags() const noexcept { return m_flags; }
        NODISCARD constexpr auto vpn() const noexcept { return m_vpn; }
        NODISCARD constexpr auto ppn() const noexcept { return m_ppn; }
    };

  private:
    PhysMemory &m_phys_memory;

    Mode m_mode = Mode::BARE;

    PPN m_table_region_begin = 0;
    PPN m_curr_table = m_table_region_begin;
    PPN m_table_region_end = 0;

  public:
    // Create SimpleMemoryMapper.
    // Pages with PPNs [table_region_begin, table_region_end) are allocated for translation tables.
    // Root translation table is placed in table_region_begin page
    SimpleMemoryMapper(PhysMemory &phys_memory, Mode mode,
                       PPN table_region_begin, PPN table_region_end)
        : m_phys_memory(phys_memory), m_mode(mode),
          m_table_region_begin(table_region_begin),
          m_table_region_end(table_region_end) {
        SIM_ASSERT(table_region_begin < table_region_end);
    }

    // Add PTEs for given mapping
    NODISCARD MapStatus map(MemoryMapping mapping) noexcept;
};

} // namespace sim::memory

#endif // INCL_MEMORY_SIMPLE_MAPPER_HPP
