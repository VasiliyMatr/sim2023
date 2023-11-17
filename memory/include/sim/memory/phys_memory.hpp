#ifndef INCL_MEMORY_PHYS_MEMORY_HPP
#define INCL_MEMORY_PHYS_MEMORY_HPP

#include <initializer_list>
#include <memory>
#include <unordered_map>

#include <sim/memory/common.hpp>

namespace sim::memory {

using HostPtr = uint8_t *;
using ConstHostPtr = const uint8_t *;

class RAM final {
    using HostPage = std::array<uint8_t, PAGE_SIZE>;
    std::unordered_map<PhysAddr, std::unique_ptr<HostPage>> m_ram{};

  public:
    NODISCARD bool addPage(PhysAddr page_pa) {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        return m_ram.try_emplace(page_pa, std::make_unique<HostPage>()).second;
    }

    NODISCARD ConstHostPtr
    getConstHostPagePtr(PhysAddr page_pa) const noexcept {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        auto it = m_ram.find(page_pa);
        if (it == m_ram.cend()) {
            return nullptr;
        }

        return it->second->data();
    }

    NODISCARD HostPtr getHostPagePtr(PhysAddr page_pa) noexcept {
        SIM_ASSERT(!(page_pa & PAGE_OFFSET_MASK));

        auto it = m_ram.find(page_pa);
        if (it == m_ram.end()) {
            return nullptr;
        }

        return it->second->data();
    }
};

struct PhysMemory final {
    enum class AccessStatus { OK, RANGE_ERROR, PAGE_ALIGN_ERROR };

  private:
    RAM m_ram{};

  public:
    NODISCARD bool addRAMPage(PhysAddr page_pa) {
        return m_ram.addPage(page_pa);
    }

    template <class UInt> struct ReadResult final {
        static_assert(std::is_unsigned_v<UInt>);

        AccessStatus status;
        ConstHostPtr host_page_ptr;
    };

    template <class UInt>
    NODISCARD ReadResult<UInt> read(PhysAddr phys_addr,
                                    UInt &dst) const noexcept {
        static_assert(std::is_unsigned_v<UInt>);

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

    template <class UInt> struct WriteResult final {
        static_assert(std::is_unsigned_v<UInt>);

        AccessStatus status;
        HostPtr host_page_ptr;
    };

    template <class UInt>
    NODISCARD WriteResult<UInt> write(PhysAddr phys_addr, UInt value) {
        static_assert(std::is_unsigned_v<UInt>);

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
