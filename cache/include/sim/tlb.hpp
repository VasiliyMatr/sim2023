#ifndef INCL_SIM_TLB_HPP
#define INCL_SIM_TLB_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim::cache {

template <class HostPtr, bit::BitSize N_LOG_2> class TLB final {
    static constexpr VirtAddr POISON_VA = 1ULL << 56;
    static constexpr size_t N = 1ULL << N_LOG_2;

    struct Entry final {
        HostPtr host = nullptr;
        VirtAddr virt_addr = POISON_VA;
    };

    Entry m_entries[N]{};

  public:
    void invalidate() noexcept {
        for (auto &&entry : m_entries) {
            entry.virt_addr = POISON_VA;
        }
    }

    bool find(VirtAddr virt_addr, HostPtr &host) {
        Entry &e = m_entries[bit::getBitField(
            memory::PAGE_BIT_SIZE + N_LOG_2, memory::PAGE_BIT_SIZE, virt_addr)];
        host = e.host + (virt_addr & memory::PAGE_OFFSET_MASK);
        return (virt_addr & ~memory::PAGE_OFFSET_MASK) == e.virt_addr;
    }

    void update(VirtAddr virt_addr, HostPtr host) {
        Entry &e = m_entries[bit::getBitField(
            memory::PAGE_BIT_SIZE + N_LOG_2, memory::PAGE_BIT_SIZE, virt_addr)];
        size_t offset = virt_addr & memory::PAGE_OFFSET_MASK;
        e.virt_addr = virt_addr - offset;
        e.host = host - offset;
    }
};

} // namespace sim::cache

#endif // INCL_SIM_TLB_HPP
