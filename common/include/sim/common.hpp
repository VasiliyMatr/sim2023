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
                                                                               \
            std::terminate();                                                  \
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
template <class UInt> constexpr UInt onesMask() noexcept {
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

    // Zero upper bits
    value &= (UInt{1} << (sign_idx + 1)) - 1;

    UInt sign_mask = UInt{1} << sign_idx;
    return (value ^ sign_mask) - sign_mask;
}

// Same as constexpr version, but sign index is passed as function argument.
// Use constexpr version if possible.
template <class UInt> UInt signExtend(BitIdx sign_idx, UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    SIM_ASSERT(sign_idx < bitSize<UInt>());

    // Zero upper bits
    value &= (UInt{1} << (sign_idx + 1)) - 1;

    UInt sign_mask = UInt{1} << sign_idx;
    return (value ^ sign_mask) - sign_mask;
}

// Get value of bit field that is stored in range [hi, lo]
template <class UInt, BitIdx hi, BitIdx lo>
constexpr UInt getBitField(UInt value) noexcept {
    size_t left_shift = bitSize<UInt>() - hi - 1;
    return value << left_shift >> (left_shift + lo);
}

// Bits out of range [hi, lo] are zeroed
template <class UInt, BitIdx hi, BitIdx lo>
constexpr UInt maskBits(UInt value) noexcept {
    return getBitField<UInt,hi, lo>(value) << lo;
}

} // namespace bit

} // namespace sim

#endif // INCL_COMMON_HPP
