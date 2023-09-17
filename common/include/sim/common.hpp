#ifndef INCL_COMMON_HPP
#define INCL_COMMON_HPP

#include <cstdint>

#define NODISCARD [[nodiscard]]

namespace sim {

using std::size_t;

using RegValue = uint64_t;
using PhysAddr = RegValue;
using VirtAddr = RegValue;

using InstrCode = uint32_t;

} // namespace sim

#endif // INCL_COMMON_HPP
