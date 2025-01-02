#ifndef INCL_MEMORY_PHYS_MEMORY_HPP
#define INCL_MEMORY_PHYS_MEMORY_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include <sys/mman.h>

#include <sim/memory/common.hpp>

namespace sim::memory {

// Pointer to host memory. Provided for fast access
using HostPtr = uint8_t *;
// Pointer to host memory. Provided for fast access
using ConstHostPtr = const uint8_t *;

// mmap/munmap wrapper
class MMap final {
    PPN m_ppn = 0;
    uint8_t *m_ptr = nullptr;

  public:
    MMap(PPN ppn)
        : m_ppn(ppn), m_ptr(static_cast<uint8_t *>(
                          mmap(nullptr, ppn * PAGE_SIZE, PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_PRIVATE, -1, 0))) {
        SIM_ASSERT(ppn != 0);

        if (m_ptr == MAP_FAILED) {
            throw std::bad_alloc();
        }
    }

    ~MMap() {
        if (m_ptr != nullptr) {
            munmap(m_ptr, m_ppn * PAGE_SIZE);
        }
    }

    MMap(const MMap &) = delete;
    MMap &operator=(const MMap &) = delete;

    MMap(MMap &&rhs) noexcept {
        std::swap(m_ppn, rhs.m_ppn);
        std::swap(m_ptr, rhs.m_ptr);
    }

    MMap &operator=(MMap &&rhs) noexcept {
        std::swap(m_ppn, rhs.m_ppn);
        std::swap(m_ptr, rhs.m_ptr);

        return *this;
    }

    auto *ptr() noexcept { return m_ptr; }
    auto ppn() noexcept { return m_ppn; }
};

// Host pages allocator
class PageAllocator final {
    std::vector<MMap> m_mmaps{};
    PPN m_curr_ppn = 0;

  public:
    PageAllocator(PPN ppn) {
        SIM_ASSERT(ppn != 0);
        m_mmaps.emplace_back(ppn);
    }

    uint8_t *allocPage() {
        static constexpr size_t ALLOC_FACTOR = 2;

        PPN end_ppn = m_mmaps.back().ppn();

        if (m_curr_ppn == end_ppn) {
            PPN new_ppn = end_ppn * ALLOC_FACTOR;
            m_mmaps.emplace_back(MMap{new_ppn});
            m_curr_ppn = 0;
        }

        return m_mmaps.back().ptr() + m_curr_ppn++ * PAGE_SIZE;
    }
};

// Random access memory. Maps RAM pages to host pages
class RAM final {
    static constexpr PPN PPN_16MB = PPN{1} << 12;

    PageAllocator m_page_allocator{PPN_16MB};
    std::unordered_map<PhysAddr, HostPtr> m_mapping{};

  public:
    // Add RAM page to mapping
    NODISCARD bool addPage(PhysAddr page_pa) {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        return m_mapping.try_emplace(page_pa, m_page_allocator.allocPage())
            .second;
    }

    // Get address of host page, mapped with given RAM page
    NODISCARD ConstHostPtr
    getConstHostPagePtr(PhysAddr page_pa) const noexcept {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        auto it = m_mapping.find(page_pa);
        if (it == m_mapping.cend()) {
            return nullptr;
        }

        return it->second;
    }

    // Get address of host page, mapped with given RAM page
    NODISCARD HostPtr getHostPagePtr(PhysAddr page_pa) noexcept {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        auto it = m_mapping.find(page_pa);
        if (it == m_mapping.end()) {
            return nullptr;
        }

        return it->second;
    }
};

// Physical memory. Provides methods for:
// - Mapping additions
// - Read/write
// - Accessed host pages addresses forwarding (when present)
class PhysMemory final {
    RAM m_ram{};

  public:
    // Add RAM memory page
    NODISCARD bool addRAMPage(PhysAddr page_pa) {
        return m_ram.addPage(page_pa);
    }

    // Physical memory read access result
    struct ReadResult final {
        SimStatus status = SimStatus::PHYS_MEM__ACCESS_FAULT;
        // Accessed host page address (when present) or nullptr
        ConstHostPtr host_page_ptr = nullptr;
    };

    // Read UInt value at given address
    template <class UInt>
    NODISCARD ReadResult read(PhysAddr phys_addr, UInt &dst) const noexcept {
        static_assert(std::is_integral_v<UInt>);

        PhysAddr page_offset = phys_addr & PAGE_OFFSET_MASK;
        if (page_offset + sizeof(UInt) > PAGE_SIZE) {
            return {SimStatus::PHYS_MEM__PAGE_ALIGN_ERROR, nullptr};
        }

        PhysAddr page_pa = phys_addr & ~PAGE_OFFSET_MASK;
        auto host_page_ptr = m_ram.getConstHostPagePtr(page_pa);
        if (host_page_ptr != nullptr) {
            dst = *reinterpret_cast<const UInt *>(host_page_ptr + page_offset);
            return {SimStatus::OK, host_page_ptr};
        }

        return {SimStatus::PHYS_MEM__ACCESS_FAULT, nullptr};
    }

    // Physical memory write access result
    struct WriteResult final {
        SimStatus status = SimStatus::PHYS_MEM__ACCESS_FAULT;
        // Accessed host page address (when present) or nullptr
        HostPtr host_page_ptr = nullptr;
    };

    // Write UInt value at given address
    template <class UInt>
    NODISCARD WriteResult write(PhysAddr phys_addr, UInt value) {
        static_assert(std::is_integral_v<UInt>);

        PhysAddr page_offset = phys_addr & PAGE_OFFSET_MASK;
        if (page_offset + sizeof(UInt) > PAGE_SIZE) {
            return {SimStatus::PHYS_MEM__PAGE_ALIGN_ERROR, nullptr};
        }

        PhysAddr page_pa = phys_addr & ~PAGE_OFFSET_MASK;
        auto host_page_ptr = m_ram.getHostPagePtr(page_pa);
        if (host_page_ptr != nullptr) {
            *reinterpret_cast<UInt *>(host_page_ptr + page_offset) = value;
            return {SimStatus::OK, host_page_ptr};
        }

        return {SimStatus::PHYS_MEM__ACCESS_FAULT, nullptr};
    }
};

} // namespace sim::memory

#endif // INCL_MEMORY_PHYS_MEMORY_HPP
