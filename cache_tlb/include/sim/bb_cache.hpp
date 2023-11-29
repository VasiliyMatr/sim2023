#ifndef INCL_CACHE_BB_HPP
#define INCL_CACHE_BB_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim {
namespace cache {

static constexpr size_t BB_SIZE_LOG_2 = 7;

template <size_t N_LOG_2> class BbCache final {
    static constexpr size_t N = 1ULL << N_LOG_2;

    bb::Bb m_entries[N]{};

  public:
    void invalidate() noexcept {
        for (auto &&bb : m_entries) {
            bb.invalidate();
        }
    }

    bb::Bb &find(VirtAddr virt_addr) {
        return m_entries[bit::getBitField(2 + N_LOG_2, 2, virt_addr)];
    }
};

} // namespace cache
} // namespace sim
#endif // INCL_CACHE_BB_HPP
