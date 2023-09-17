#ifndef INCL_GPR_HPP
#define INCL_GPR_HPP

#include <array>
#include <cassert>
#include <cstdint>

#include <sim/common.hpp>

namespace sim {
namespace gpr {

inline constexpr std::size_t GPR_NUMBER = 32;

class GPRFile final {
    std::array<RegValue, GPR_NUMBER> m_gpr{};

  public:
    template <class T> void write(size_t idx, T value) noexcept;
    template <class T> NODISCARD T read(size_t idx) const noexcept;
};

} // namespace gpr
} // namespace sim

#endif // INCL_GPR_HPP
