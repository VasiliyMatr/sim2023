#ifndef INCL_SIM_BB_CACHE_HPP
#define INCL_SIM_BB_CACHE_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <sim/bb.hpp>
#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim::cache {

template <bit::BitSize N_LOG_2> class BbCache final {
    static constexpr size_t N = 1ULL << N_LOG_2;
    static constexpr bit::BitSize PC_ALIGN_BITS = 2;

    std::array<bb::Bb, N> m_entries{};

  public:
    void invalidate() noexcept {
        for (auto &&bb : m_entries) {
            bb.invalidate();
        }
    }

    bb::Bb &find(VirtAddr virt_addr) {
        return m_entries[bit::getBitField(PC_ALIGN_BITS + N_LOG_2 - 1,
                                          PC_ALIGN_BITS, virt_addr)];
    }
};

} // namespace sim::cache

#endif // INCL_SIM_BB_CACHE_HPP
