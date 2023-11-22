#ifndef INCL_MEMORY_HPP
#define INCL_MEMORY_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <sim/common.hpp>

namespace sim {
namespace memory {

struct PhysMemory final {
    enum class AccessStatus { OK, RANGE_ERROR };

  private:
    std::vector<uint8_t> m_data{};
    PhysAddr m_base_addr = 0;

    NODISCARD auto getAccessStatus(PhysAddr phys_addr,
                                   size_t access_size) const noexcept {
        SIM_ASSERT(access_size > 0);

        if (phys_addr < m_base_addr) {
            return AccessStatus::RANGE_ERROR;
        }

        auto offset = phys_addr - m_base_addr;
        if (offset + access_size > m_data.size()) {
            return AccessStatus::RANGE_ERROR;
        }

        return AccessStatus::OK;
    }

  public:
    PhysMemory(PhysAddr base_addr, size_t size)
        : m_base_addr(base_addr), m_data(size) {}

    NODISCARD auto baseAddr() const noexcept { return m_base_addr; }
    NODISCARD auto size() const noexcept { return m_data.size(); }

    template <class UInt>
    NODISCARD AccessStatus read(PhysAddr phys_addr, UInt &dst) const noexcept {
        static_assert(std::is_integral_v<UInt>);

        if (auto status = getAccessStatus(phys_addr, sizeof(UInt));
            status != AccessStatus::OK) {
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        dst = *reinterpret_cast<const UInt *>(m_data.data() + offset);

        return AccessStatus::OK;
    }

    template <class UInt>
    NODISCARD AccessStatus write(PhysAddr phys_addr, UInt value) {
        static_assert(std::is_integral_v<UInt>);

        if (auto status = getAccessStatus(phys_addr, sizeof(UInt));
            status != AccessStatus::OK) {
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        *reinterpret_cast<UInt *>(m_data.data() + offset) = value;

        return AccessStatus::OK;
    }

    NODISCARD AccessStatus
    getConstHostPtr(PhysAddr phys_addr, size_t access_size,
                    const uint8_t *&host_ptr) const noexcept {
        SIM_ASSERT(access_size > 0);

        if (auto status = getAccessStatus(phys_addr, access_size);
            status != AccessStatus::OK) {
            host_ptr = nullptr;
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        host_ptr = m_data.data() + offset;
        return AccessStatus::OK;
    }

    NODISCARD AccessStatus getMuteHostPtr(PhysAddr phys_addr,
                                          size_t access_size,
                                          uint8_t *&host_ptr) noexcept {
        SIM_ASSERT(access_size > 0);

        if (auto status = getAccessStatus(phys_addr, access_size);
            status != AccessStatus::OK) {
            host_ptr = nullptr;
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        host_ptr = m_data.data() + offset;
        return AccessStatus::OK;
    }
};

} // namespace memory
} // namespace sim

#endif // INCL_MEMORY_HPP
