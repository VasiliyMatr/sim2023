#ifndef INCL_MEMORY_HPP
#define INCL_MEMORY_HPP

#include <sim/common.hpp>
#include <type_traits>
#include <vector>

namespace sim {
namespace memory {

class PhysMemory final {
    std::vector<uint8_t> m_data{};
    PhysAddr m_base_addr = 0;

    template<class UInt>
    NODISCARD auto calcAndCheckOffset(PhysAddr phys_addr) const noexcept {
        static_assert(std::is_unsigned_v<UInt>);
        SIM_ASSERT(phys_addr >= m_base_addr);

        auto offset = phys_addr - m_base_addr;
        SIM_ASSERT(offset + sizeof(UInt) < m_data.size());

        return offset;
    }

  public:
    PhysMemory(PhysAddr base_addr, size_t size) : m_base_addr(base_addr), m_data(size) {}

    NODISCARD auto baseAddr() const noexcept { return m_base_addr; }
    template<class UInt>
    NODISCARD auto read(PhysAddr phys_addr) const noexcept {
        static_assert(std::is_unsigned_v<UInt>);
        auto offset = calcAndCheckOffset<UInt>(phys_addr);
        return *reinterpret_cast<const UInt*>(m_data.data() + offset);
    }
    template<class UInt>
    void write(PhysAddr phys_addr, UInt value) {
        static_assert(std::is_unsigned_v<UInt>);
        auto offset = calcAndCheckOffset<UInt>(phys_addr);
        *reinterpret_cast<const UInt*>(m_data.data() + offset) = value;
    }
};

} // namespace memory
} // namespace sim

#endif // INCL_MEMORY_HPP
