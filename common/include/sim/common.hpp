#ifndef INCL_COMMON_HPP
#define INCL_COMMON_HPP

#include <climits>
#include <cstdint>
#include <iostream>
#include <type_traits>

#define NODISCARD [[nodiscard]]

#ifndef SIM_ASSERT
#define SIM_ASSERT(cond)                                                       \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::cerr << "Assertion failed: " << #cond << std::endl            \
                      << "Location: " << __FILE__ << ":" << __LINE__           \
                      << std::endl;                                            \
        }                                                                      \
    } while (0)
#endif

namespace sim {

using std::size_t;

using RegValue = uint64_t;
using PhysAddr = RegValue;
using VirtAddr = RegValue;

using InstrCode = uint32_t;

namespace bit {

using BitIdx = size_t;
using BitSize = size_t;

static constexpr size_t BYTE_SIZE = 8;
static_assert(CHAR_BIT == BYTE_SIZE);

// Return ones mask for given unsigned integer type
template <class UInt> constexpr UInt ones() noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    return UInt{0} - 1;
}

// Return given unsigned integer type size in bits
template <class UInt> constexpr BitSize bitSize() noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    return sizeof(UInt) * BYTE_SIZE;
}

// Sign-extend given unsigned integer value from a given bit index.
// Bits that are higher than the sign bit are ignored.
template <class UInt, BitIdx sign_idx>
constexpr UInt signExtend(UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(sign_idx < bitSize<UInt>());

    UInt masked_sign = value & (UInt{1} << sign_idx);
    return value | ~(masked_sign - 1);
}

// Same as constexpr version, but sign index is passed as function argument.
// Use constexpr version if possible.
template <class UInt> UInt signExtend(BitIdx sign_idx, UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    SIM_ASSERT(sign_idx < bitSize<UInt>());

    UInt masked_sign = value & (UInt{1} << sign_idx);
    return value | ~(masked_sign - 1);
}

// Return bits in range [hi, lo] of a given value. Other bits are zeroed.
template <class UInt, BitIdx hi, BitIdx lo>
constexpr UInt getBits(UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(hi < bitSize<UInt>());
    static_assert(lo <= hi);

    UInt mask = (UInt{1} << hi) ^ (UInt{1} << lo);
    return value & mask;
}

// Return bits in range [hi, lo] of a given value. Other bits are zeroed.
// Returned value is shifted to the right by lo bits.
template <class UInt, BitIdx hi, BitIdx lo>
constexpr UInt getBitsShifted(UInt value) noexcept {
    return getBits<UInt, hi, lo>(value) >> lo;
}

} // namespace bit

} // namespace sim

#endif // INCL_COMMON_HPP
