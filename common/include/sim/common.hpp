#ifndef INCL_SIM_COMMON_HPP
#define INCL_SIM_COMMON_HPP

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

template <class Enum> constexpr auto to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}

// Simulation status codes
enum class SimStatus {
    OK,

    // *** csr:: codes ***
    CSR__NOT_SUPPORTED,

    // *** memory::PhysMemory codes ***
    PHYS_MEM__PAGE_ALIGN_ERROR,
    PHYS_MEM__ACCESS_FAULT,

    // *** memory::MMU64 codes ***
    MMU64__PAGE_FAULT,

    // *** memory::SimpleMemoryMapper codes ***
    MAPPER__ALREADY_MAPPED,
    MAPPER__TABLE_REGION_END,
    MAPPER__TABLE_REGION_PAGE_MAPPED,

    // Simulator codes
    SIM__EXIT,
    SIM__NOT_IMPLEMENTED_INSTR,
    SIM__PC_ALIGN_ERROR
};

enum class XLen { XLEN_32 = 32, XLEN_64 = 64 };

using RegValue = uint64_t;
using PhysAddr = RegValue;
using VirtAddr = RegValue;

using InstrCode = uint32_t;
static constexpr size_t INSTR_CODE_SIZE = sizeof(InstrCode);

enum class PrivLevel { USER = 0b00, SUPERVISOR = 0b01, MACHINE = 0b11 };

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
template <class UInt>
constexpr UInt signExtend(BitIdx sign_idx, UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);
    SIM_ASSERT(sign_idx < bitSize<UInt>());

    // Avoid UB when shifting by sign_idx + 1
    if (sign_idx == bitSize<UInt>() - 1) {
        return value;
    }

    // Zero upper bits
    value &= (UInt{1} << (sign_idx + 1)) - 1;

    UInt sign_mask = UInt{1} << sign_idx;
    return (value ^ sign_mask) - sign_mask;
}

// Get value of bit field that is stored in range [hi, lo]
template <class UInt>
constexpr UInt getBitField(BitIdx hi, BitIdx lo, UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);

    SIM_ASSERT(bitSize<UInt>() > hi);
    SIM_ASSERT(hi >= lo);

    size_t left_shift = bitSize<UInt>() - hi - 1;
    return value << left_shift >> (left_shift + lo);
}

// Set bits [hi, lo]
template <class UInt>
constexpr UInt setBitField(BitIdx hi, BitIdx lo, UInt value,
                           UInt bit_field) noexcept {
    static_assert(std::is_unsigned_v<UInt>);

    SIM_ASSERT(bitSize<UInt>() > hi);
    SIM_ASSERT(hi >= lo);

    UInt hi_shift = hi + 1 == bitSize<UInt>() ? 0 : (UInt{1} << (hi + 1));
    UInt bit_field_mask = hi_shift - (UInt{1} << lo);

    bit_field = (bit_field << lo) & bit_field_mask;
    return (value & ~bit_field_mask) | bit_field;
}

// Bits out of range [hi, lo] are zeroed
template <class UInt>
constexpr UInt maskBits(BitIdx hi, BitIdx lo, UInt value) noexcept {
    static_assert(std::is_unsigned_v<UInt>);

    SIM_ASSERT(bitSize<UInt>() > hi);
    SIM_ASSERT(hi >= lo);

    return getBitField<UInt>(hi, lo, value) << lo;
}

} // namespace bit

} // namespace sim

#endif // INCL_SIM_COMMON_HPP
