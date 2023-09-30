#include <cstdint>
#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include <sim/common.hpp>
#include <sim/memory.hpp>
#include <unordered_map>

namespace sim {
namespace memory {

namespace {

constexpr PhysAddr DATA_SEGMENT_BASE_ADDR = 0x5000000000;
constexpr size_t SIZE_16MB = size_t{1} << 24;
constexpr size_t SIZE_2GB = size_t{1} << 31;

class PhysMemoryTest : public ::testing::Test {
  protected:
    static constexpr uint64_t MT_SEED = 1003;

    std::mt19937_64 mt{MT_SEED};
};

} // namespace

TEST_F(PhysMemoryTest, readWrite) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    std::unordered_map<PhysAddr, uint64_t> test_cases = {
        {DATA_SEGMENT_BASE_ADDR, mt()},
        {DATA_SEGMENT_BASE_ADDR + 0x1000, mt()},
        {DATA_SEGMENT_BASE_ADDR + SIZE_16MB - sizeof(uint64_t), mt()}};

    for (auto &test_case : test_cases) {
        ASSERT_EQ(pm.write(test_case.first, test_case.second),
                  PhysMemory::AccessStatus::OK);
    }

    for (auto &test_case : test_cases) {
        PhysAddr pa = test_case.first;
        uint64_t value = test_case.second;

        uint64_t read_value_64 = 0;
        ASSERT_EQ(pm.read(pa, read_value_64), PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_64, value);

        uint32_t read_value_32 = 0;
        ASSERT_EQ(pm.read(pa, read_value_32), PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_32, static_cast<uint32_t>(value));

        uint16_t read_value_16 = 0;
        ASSERT_EQ(pm.read(pa, read_value_16), PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_16, static_cast<uint16_t>(value));

        uint8_t read_value_8 = 0;
        ASSERT_EQ(pm.read(pa, read_value_8), PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_8, static_cast<uint8_t>(value));
    }
}

namespace {

const std::unordered_map<PhysAddr, const char *> RANGE_ERROR_TEST_CASES = {
    {0, "Under segment base addr"},
    {DATA_SEGMENT_BASE_ADDR + SIZE_16MB + 10, "Over segment max addr"},
    {DATA_SEGMENT_BASE_ADDR + SIZE_16MB - 4, "On segment bound"}};

} // namespace

TEST_F(PhysMemoryTest, rangeErrorReadWrite) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    for (const auto &test_case : RANGE_ERROR_TEST_CASES) {
        const char *descr = test_case.second;
        PhysAddr pa = test_case.first;

        uint64_t _ = 0;

        ASSERT_EQ(pm.read(pa, _), PhysMemory::AccessStatus::RANGE_ERROR)
            << descr;
        ASSERT_EQ(pm.write(pa, uint64_t{}),
                  PhysMemory::AccessStatus::RANGE_ERROR)
            << descr;
    }
}

TEST_F(PhysMemoryTest, rangeErrorHostPtr) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    for (const auto &test_case : RANGE_ERROR_TEST_CASES) {
        const char *descr = test_case.second;
        PhysAddr pa = test_case.first;

        uint8_t _ = 0;

        const uint8_t *const_host_ptr = &_;
        ASSERT_EQ(pm.getConstHostPtr(pa, 8, const_host_ptr),
                  PhysMemory::AccessStatus::RANGE_ERROR)
            << descr;
        ASSERT_EQ(const_host_ptr, nullptr) << descr;

        uint8_t *mute_host_ptr = &_;
        ASSERT_EQ(pm.getMuteHostPtr(pa, 8, mute_host_ptr),
                  PhysMemory::AccessStatus::RANGE_ERROR)
            << descr;
        ASSERT_EQ(mute_host_ptr, nullptr) << descr;
    }
}

TEST_F(PhysMemoryTest, constHostAddr) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + 0x1000;
    const uint8_t *host_ptr = nullptr;

    ASSERT_EQ(pm.getConstHostPtr(pa, 2, host_ptr),
              PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_ptr != nullptr);

    uint8_t write_value1 = mt();
    uint8_t write_value2 = mt();

    ASSERT_EQ(pm.write(pa, write_value1), PhysMemory::AccessStatus::OK);
    ASSERT_EQ(pm.write(pa + 1, write_value2), PhysMemory::AccessStatus::OK);
    ASSERT_EQ(host_ptr[0], write_value1);
    ASSERT_EQ(host_ptr[1], write_value2);
}

TEST_F(PhysMemoryTest, muteHostAddr) {
    PhysMemory pm{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + 0x1000;
    uint8_t *host_ptr = nullptr;

    ASSERT_EQ(pm.getMuteHostPtr(pa, 2, host_ptr), PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_ptr != nullptr);

    host_ptr[0] = mt();
    host_ptr[1] = mt();
    uint8_t read_value1 = 0;
    uint8_t read_value2 = 0;

    ASSERT_EQ(pm.read(pa, read_value1), PhysMemory::AccessStatus::OK);
    ASSERT_EQ(pm.read(pa + 1, read_value2), PhysMemory::AccessStatus::OK);
    ASSERT_EQ(read_value1, host_ptr[0]);
    ASSERT_EQ(read_value2, host_ptr[1]);
}

} // namespace memory
} // namespace sim
