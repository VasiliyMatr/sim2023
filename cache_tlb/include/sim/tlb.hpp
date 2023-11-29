#ifndef INCL_CACHE_TLB_HPP
#define INCL_CACHE_TLB_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim {
namespace cache {

static constexpr VirtAddr POISON_VA = 1ULL << 56;
static constexpr size_t TLB_SIZE_LOG_2 = 7;

template <size_t N_LOG_2> class TLB final {
    static constexpr size_t N = 1ULL << N_LOG_2;

    struct Entry final {
        memory::HostPtr host = nullptr;
        VirtAddr virt_addr = POISON_VA;
    };

    Entry m_entries[N]{};

  public:
    void invalidate() noexcept {
        for (auto &&entry : m_entries) {
            entry.virt_addr = POISON_VA;
        }
    }

    bool find(VirtAddr virt_addr, memory::HostPtr &host) {
        Entry &e = m_entries[bit::getBitField(memory::PAGE_BIT_SIZE + N_LOG_2, memory::PAGE_BIT_SIZE,
                                         virt_addr)];
        host = e.host + (virt_addr & memory::PAGE_OFFSET_MASK);
        return (virt_addr & ~memory::PAGE_OFFSET_MASK) == e.virt_addr;
    }

    void update(VirtAddr virt_addr, memory::HostPtr host) {
        Entry &e = m_entries[bit::getBitField(memory::PAGE_BIT_SIZE + N_LOG_2, memory::PAGE_BIT_SIZE,
                                         virt_addr)];
        size_t offset = virt_addr & memory::PAGE_OFFSET_MASK;
        e.virt_addr = virt_addr - offset;
        e.host = host - offset;
    }
};

} // namespace cache
} // namespace sim
#endif // INCL_CACHE_TLB_HPP
