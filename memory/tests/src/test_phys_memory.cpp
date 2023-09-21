#include <cstdint>
#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim {
namespace memory {

static constexpr PhysAddr DATA_SEGMENT_BASE_ADDR = 0x5000000000;
static constexpr size_t SIZE_16MB = size_t{1} << 24;
static constexpr size_t SIZE_2GB = size_t{1} << 31;

class PhysMemoryTest : public ::testing::Test {
  protected:
    static constexpr uint64_t MT_SEED = 1003;

    std::mt19937_64 mt{MT_SEED};
};

TEST_F(PhysMemoryTest, readWrite) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    std::vector<PhysAddr> test_phys_addrs = {
        DATA_SEGMENT_BASE_ADDR, DATA_SEGMENT_BASE_ADDR + 0x0FF5E1,
        DATA_SEGMENT_BASE_ADDR + SIZE_16MB - sizeof(uint64_t)};

    std::vector<uint64_t> test_values = {mt(), mt(), mt()};

    ASSERT_EQ(test_phys_addrs.size(), test_values.size());

    for (size_t i = 0, end = test_phys_addrs.size(); i != end; ++i) {
        pm.write(test_phys_addrs[i], uint64_t{test_values[i]});
    }

    for (size_t i = 0, end = test_phys_addrs.size(); i != end; ++i) {
        PhysAddr pa = test_phys_addrs[i];
        uint64_t value = test_values[i];

        ASSERT_EQ(pm.read<uint64_t>(pa), value);
        ASSERT_EQ(pm.read<uint32_t>(pa), static_cast<uint32_t>(value));
        ASSERT_EQ(pm.read<uint16_t>(pa), static_cast<uint16_t>(value));
        ASSERT_EQ(pm.read<uint8_t>(pa), static_cast<uint8_t>(value));
    }
}

TEST(PhysMemory, outOfRange1) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    auto matcher =
        ::testing::HasSubstr("Assertion failed: phys_addr >= m_base_addr");

    PhysAddr pa = 0x0;
    ASSERT_DEATH(std::ignore = pm.read<uint64_t>(pa), matcher);
    ASSERT_DEATH(pm.write(pa, uint64_t{}), matcher);
}

TEST(PhysMemory, outOfRange2) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    auto matcher = ::testing::HasSubstr(
        "Assertion failed: offset + sizeof(UInt) <= m_data.size()");

    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + SIZE_16MB + 10;
    ASSERT_DEATH(std::ignore = pm.read<uint64_t>(pa), matcher);
    ASSERT_DEATH(pm.write(pa, uint64_t{}), matcher);
}

TEST(PhysMemory, outOfRange3) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    auto matcher = ::testing::HasSubstr(
        "Assertion failed: offset + sizeof(UInt) <= m_data.size()");

    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + SIZE_16MB - 4;
    ASSERT_DEATH(std::ignore = pm.read<uint64_t>(pa), matcher);
    ASSERT_DEATH(pm.write(pa, uint64_t{}), matcher);
}

} // namespace memory
} // namespace sim
