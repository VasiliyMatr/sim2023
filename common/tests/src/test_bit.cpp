
#include <cstdint>
#include <gtest/gtest.h>

#include <sim/common.hpp>

namespace sim {
namespace bit {

TEST(Bit, onesMask) {
    ASSERT_EQ(onesMask<uint8_t>(), 0xFF);
    ASSERT_EQ(onesMask<uint16_t>(), 0xFFFF);
    ASSERT_EQ(onesMask<uint32_t>(), 0xFFFFFFFF);
    ASSERT_EQ(onesMask<uint64_t>(), 0xFFFFFFFFFFFFFFFF);
}

TEST(Bit, bitSize) {
    ASSERT_EQ(bitSize<uint8_t>(), 8);
    ASSERT_EQ(bitSize<uint16_t>(), 16);
    ASSERT_EQ(bitSize<uint32_t>(), 32);
    ASSERT_EQ(bitSize<uint64_t>(), 64);
}

TEST(Bit, signExtend) {
    ASSERT_EQ((signExtend<uint32_t, 3>(0b1010)), 0xFFFFFFFA);
    ASSERT_EQ((signExtend<uint32_t, 2>(0b1010)), 0x2);
    ASSERT_EQ((signExtend<uint64_t, 7>(0b10101010)), 0xFFFFFFFFFFFFFFAA);
    ASSERT_EQ((signExtend<uint64_t, 6>(0b10101010)), 0x2A);
}

TEST(Bit, signExtendWithArg) {
    ASSERT_EQ((signExtend<uint32_t>(3, 0b1010)), 0xFFFFFFFA);
    ASSERT_EQ((signExtend<uint32_t>(2, 0b1010)), 0x2);
    ASSERT_EQ((signExtend<uint64_t>(7, 0b10101010)), 0xFFFFFFFFFFFFFFAA);
    ASSERT_EQ((signExtend<uint64_t>(6, 0b10101010)), 0x2A);
}

TEST(Bit, maskBits) {
    ASSERT_EQ((maskBits<uint32_t, 7, 2>(0b10111011)), 0b10111000);
    ASSERT_EQ((maskBits<uint64_t, 13, 11>(0xABBA)), 0x2800);
}

TEST(Bit, getBitField) {
    ASSERT_EQ((getBitField<uint32_t, 7, 2>(0b10111011)), 0b101110);
    ASSERT_EQ((getBitField<uint64_t, 13, 11>(0xABBA)), 0b101);
}

} // namespace bit
} // namespace sim
