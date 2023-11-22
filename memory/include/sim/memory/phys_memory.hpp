#ifndef INCL_MEMORY_PHYS_MEMORY_HPP
#define INCL_MEMORY_PHYS_MEMORY_HPP

#include <initializer_list>
#include <memory>
#include <unordered_map>

#include <sim/memory/common.hpp>

namespace sim::memory {

// Pointer to host memory. Provided for fast access
using HostPtr = uint8_t *;
// Pointer to host memory. Provided for fast access
using ConstHostPtr = const uint8_t *;

// Random access memory. Maps RAM pages to host pages
class RAM final {
    using HostPage = std::array<uint8_t, PAGE_SIZE>;
    std::unordered_map<PhysAddr, std::unique_ptr<HostPage>> m_ram{};

  public:
    // Add RAM page to mapping
    NODISCARD bool addPage(PhysAddr page_pa) {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        return m_ram.try_emplace(page_pa, std::make_unique<HostPage>()).second;
    }

    // Get address of host page, mapped with given RAM page
    NODISCARD ConstHostPtr
    getConstHostPagePtr(PhysAddr page_pa) const noexcept {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        auto it = m_ram.find(page_pa);
        if (it == m_ram.cend()) {
            return nullptr;
        }

        return it->second->data();
    }

    // Get address of host page, mapped with given RAM page
    NODISCARD HostPtr getHostPagePtr(PhysAddr page_pa) noexcept {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        auto it = m_ram.find(page_pa);
        if (it == m_ram.end()) {
            return nullptr;
        }

        return it->second->data();
    }
};

// Physical memory. Provides methods for:
// - Mapping additions
// - Read/write
// - Accessed host pages addresses forwarding (when present)
struct PhysMemory final {
    // Physical memory access status
    enum class AccessStatus {
        // Successful access
        OK,
        // Unmapped range accessed
        RANGE_ERROR,
        // Access on physical pages boundary
        PAGE_ALIGN_ERROR
    };

  private:
    RAM m_ram{};

  public:
    // Add RAM memory page
    NODISCARD bool addRAMPage(PhysAddr page_pa) {
        return m_ram.addPage(page_pa);
    }

    // Physical memory read access result
    struct ReadResult final {
        AccessStatus status;
        // Accessed host page address (when present) or nullptr
        ConstHostPtr host_page_ptr;
    };

    // Read UInt value at given address
    template <class UInt>
    NODISCARD ReadResult read(PhysAddr phys_addr, UInt &dst) const noexcept {
        static_assert(std::is_integral_v<UInt>);

        PhysAddr page_offset = phys_addr & PAGE_OFFSET_MASK;
        if (page_offset + sizeof(UInt) > PAGE_SIZE) {
            return {AccessStatus::PAGE_ALIGN_ERROR, nullptr};
        }

        PhysAddr page_pa = phys_addr & ~PAGE_OFFSET_MASK;
        auto host_page_ptr = m_ram.getConstHostPagePtr(page_pa);
        if (host_page_ptr != nullptr) {
            dst = *reinterpret_cast<const UInt *>(host_page_ptr + page_offset);
            return {AccessStatus::OK, host_page_ptr};
        }

        return {AccessStatus::RANGE_ERROR, nullptr};
    }

    // Physical memory write access result
    struct WriteResult final {
        AccessStatus status;
        // Accessed host page address (when present) or nullptr
        HostPtr host_page_ptr;
    };

    // Write UInt value at given address
    template <class UInt>
    NODISCARD WriteResult write(PhysAddr phys_addr, UInt value) {
        static_assert(std::is_integral_v<UInt>);

        PhysAddr page_offset = phys_addr & PAGE_OFFSET_MASK;
        if (page_offset + sizeof(UInt) > PAGE_SIZE) {
            return {AccessStatus::PAGE_ALIGN_ERROR, nullptr};
        }

        PhysAddr page_pa = phys_addr & ~PAGE_OFFSET_MASK;
        auto host_page_ptr = m_ram.getHostPagePtr(page_pa);
        if (host_page_ptr != nullptr) {
            *reinterpret_cast<UInt *>(host_page_ptr + page_offset) = value;
            return {AccessStatus::OK, host_page_ptr};
        }

        return {AccessStatus::RANGE_ERROR, nullptr};
    }
};

} // namespace sim::memory

#endif // INCL_MEMORY_PHYS_MEMORY_HPP
