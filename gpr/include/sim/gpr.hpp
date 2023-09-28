#ifndef INCL_GPR_HPP
#define INCL_GPR_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <type_traits>

#include <sim/common.hpp>

namespace sim {
namespace gpr {

inline constexpr size_t GPR_NUMBER = 32;

namespace GPR_IDX {
enum GPR_IDX : size_t {
    ZERO = 0,
    RA = 1,
    SP = 2,
    GP = 3,
    TP = 4,
    T0 = 5,
    T1 = 6,
    T2 = 7,
    // AKA FP
    S0 = 8,
    S1 = 9,
    A0 = 10,
    A1 = 11,
    A2 = 12,
    A3 = 13,
    A4 = 14,
    A5 = 15,
    A6 = 16,
    A7 = 17,
    S2 = 18,
    S3 = 19,
    S4 = 20,
    S5 = 21,
    S6 = 22,
    S7 = 23,
    S8 = 24,
    S9 = 25,
    S10 = 26,
    S11 = 27,
    T3 = 28,
    T4 = 29,
    T5 = 30,
    T6 = 31,
};
} // namespace GPR_IDX

class GPRFile final {
    std::array<RegValue, GPR_NUMBER> m_gpr{};

  public:
    void write(size_t idx, RegValue value) noexcept {
        SIM_ASSERT(idx < GPR_NUMBER);

        m_gpr[idx] = value;
        m_gpr[GPR_IDX::ZERO] = 0;
    }

    template <class UInt> NODISCARD UInt read(size_t idx) const noexcept {
        SIM_ASSERT(idx < GPR_NUMBER);

        return m_gpr[idx];
    }
};

} // namespace gpr
} // namespace sim

#endif // INCL_GPR_HPP
