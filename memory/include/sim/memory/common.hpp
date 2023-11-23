#ifndef INCL_MEMORY_COMMON_HPP
#define INCL_MEMORY_COMMON_HPP

#include <sim/common.hpp>

namespace sim::memory {

static constexpr bit::BitSize PAGE_BIT_SIZE = 12;
static constexpr size_t PAGE_SIZE = 1 << PAGE_BIT_SIZE;
static constexpr PhysAddr PAGE_OFFSET_MASK = PAGE_SIZE - 1;

// Physical page number
using PPN = uint64_t;

// Virtual page number
using VPN = uint64_t;

} // namespace sim::memory

#endif // INCL_MEMORY_COMMON_HPP
