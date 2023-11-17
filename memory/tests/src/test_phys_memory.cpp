#include <algorithm>
#include <random>
#include <unordered_map>

#include <gtest/gtest.h>

#include <sim/common.hpp>
#include <sim/memory.hpp>

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

    PhysMemory pm{};

    PhysMemoryTest() {
        for (PhysAddr page_pa = DATA_SEGMENT_BASE_ADDR,
                      end = DATA_SEGMENT_BASE_ADDR + SIZE_16MB;
             page_pa != end; page_pa += PAGE_SIZE) {

            SIM_ASSERT(pm.addRAMPage(page_pa));
        }
    }
};

} // namespace

TEST_F(PhysMemoryTest, readWrite) {
    std::unordered_map<PhysAddr, uint64_t> test_cases = {
        {DATA_SEGMENT_BASE_ADDR, mt()},
        {DATA_SEGMENT_BASE_ADDR + 0x1000, mt()},
        {DATA_SEGMENT_BASE_ADDR + SIZE_16MB - sizeof(uint64_t), mt()}};

    for (auto &test_case : test_cases) {
        ASSERT_EQ(pm.write(test_case.first, test_case.second).status,
                  PhysMemory::AccessStatus::OK);
    }

    for (auto &test_case : test_cases) {
        PhysAddr pa = test_case.first;
        uint64_t value = test_case.second;

        uint64_t read_value_64 = 0;
        ASSERT_EQ(pm.read(pa, read_value_64).status,
                  PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_64, value);

        uint32_t read_value_32 = 0;
        ASSERT_EQ(pm.read(pa, read_value_32).status,
                  PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_32, static_cast<uint32_t>(value));

        uint16_t read_value_16 = 0;
        ASSERT_EQ(pm.read(pa, read_value_16).status,
                  PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_16, static_cast<uint16_t>(value));

        uint8_t read_value_8 = 0;
        ASSERT_EQ(pm.read(pa, read_value_8).status,
                  PhysMemory::AccessStatus::OK);
        ASSERT_EQ(read_value_8, static_cast<uint8_t>(value));
    }
}

namespace {} // namespace

TEST_F(PhysMemoryTest, rangeError) {
    const std::unordered_map<PhysAddr, const char *> RANGE_ERROR_TEST_CASES = {
        {0, "Under segment base addr"},
        {DATA_SEGMENT_BASE_ADDR + SIZE_16MB + 10, "Over segment max addr"}};

    for (const auto &test_case : RANGE_ERROR_TEST_CASES) {
        const char *descr = test_case.second;
        PhysAddr pa = test_case.first;

        uint64_t dst = 0;

        ASSERT_EQ(pm.read(pa, dst).status,
                  PhysMemory::AccessStatus::RANGE_ERROR)
            << descr;
        ASSERT_EQ(pm.write(pa, uint64_t{}).status,
                  PhysMemory::AccessStatus::RANGE_ERROR)
            << descr;
    }
}

TEST_F(PhysMemoryTest, pageAlignError) {
    const std::unordered_map<PhysAddr, const char *>
        PAGE_ALIGN_ERROR_TEST_CASES = {
            {DATA_SEGMENT_BASE_ADDR + 0x1000 - 4, "On inner page bound"},
            {DATA_SEGMENT_BASE_ADDR - 4, "On lower segment bound"},
            {DATA_SEGMENT_BASE_ADDR + SIZE_16MB - 4, "On upper segment bound"}};

    for (const auto &test_case : PAGE_ALIGN_ERROR_TEST_CASES) {
        const char *descr = test_case.second;
        PhysAddr pa = test_case.first;

        uint64_t dst = 0;

        ASSERT_EQ(pm.read(pa, dst).status,
                  PhysMemory::AccessStatus::PAGE_ALIGN_ERROR)
            << descr;
        ASSERT_EQ(pm.write(pa, uint64_t{}).status,
                  PhysMemory::AccessStatus::PAGE_ALIGN_ERROR)
            << descr;
    }
}

TEST_F(PhysMemoryTest, constHostPtr) {
    PPN ppn = 1;
    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + ppn * PAGE_SIZE;

    // Check first host page pointer
    uint8_t byte = 0;
    auto [status1, host_page_ptr1] = pm.read(pa, byte);
    ASSERT_EQ(status1, PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_page_ptr1 != nullptr);
    ASSERT_EQ(host_page_ptr1[0], byte);

    // Check second host page pointer
    PhysAddr offset = 0x0ff;
    auto [status2, host_page_ptr2] = pm.read(pa + offset, byte);
    ASSERT_EQ(status2, PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_page_ptr2 != nullptr);
    ASSERT_EQ(host_page_ptr2[offset], byte);

    // Same phys pages => same host pages
    ASSERT_EQ(host_page_ptr1, host_page_ptr2);
}

TEST_F(PhysMemoryTest, hostPtr) {
    PPN ppn = 1;
    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + ppn * PAGE_SIZE;

    // Check first host page pointer
    uint8_t byte = mt();
    auto [status1, host_page_ptr1] = pm.write(pa, byte);
    ASSERT_EQ(status1, PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_page_ptr1 != nullptr);
    ASSERT_EQ(host_page_ptr1[0], byte);

    // Check second host page pointer
    PhysAddr offset = 0x0ff;
    byte = mt();
    auto [status2, host_page_ptr2] = pm.write(pa + offset, byte);
    ASSERT_EQ(status2, PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_page_ptr2 != nullptr);
    ASSERT_EQ(host_page_ptr2[offset], byte);

    // Same phys pages => same host pages
    ASSERT_EQ(host_page_ptr1, host_page_ptr2);
}

TEST_F(PhysMemoryTest, hostPtrCoherency) {
    PPN ppn = 1;
    PhysAddr pa = DATA_SEGMENT_BASE_ADDR + ppn * PAGE_SIZE;

    // Get host ptr
    uint8_t byte = mt();
    auto [status, host_page_ptr] = pm.write(pa, byte);
    ASSERT_EQ(status, PhysMemory::AccessStatus::OK);
    ASSERT_TRUE(host_page_ptr != nullptr);
    ASSERT_EQ(host_page_ptr[0], byte);

    std::array<uint8_t, PAGE_SIZE> test_data{};

    // Check pm writes coherency
    std::for_each(test_data.begin(), test_data.end(),
                  [this](uint8_t &data) { data = mt(); });
    for (size_t i = 0; i != PAGE_SIZE; ++i) {
        ASSERT_EQ(pm.write(pa + i, test_data[i]).status,
                  PhysMemory::AccessStatus::OK)
            << i;
        ASSERT_EQ(test_data[i], host_page_ptr[i]) << i;
    }

    // Check host writes coherency
    std::for_each(test_data.begin(), test_data.end(),
                  [this](uint8_t &data) { data = mt(); });
    for (size_t i = 0; i != PAGE_SIZE; ++i) {
        host_page_ptr[i] = test_data[i];

        ASSERT_EQ(pm.read(pa + i, byte).status, PhysMemory::AccessStatus::OK)
            << i;
        ASSERT_EQ(byte, host_page_ptr[i]) << i;
    }
}

} // namespace memory
} // namespace sim
