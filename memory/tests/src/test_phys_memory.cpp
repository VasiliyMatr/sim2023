#include <algorithm>
#include <random>

#include <gtest/gtest.h>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim {
namespace memory {

using Status = PhysMemory::AccessStatus;

namespace {

// Prepare test PhysMemory and mt19937_64
class PhysMemoryTest : public ::testing::Test {
    static constexpr uint64_t MT_SEED = 1003;

  protected:
    static constexpr PhysAddr RAM_BASE_PA = 0x5000000000;
    static constexpr size_t RAM_SIZE_16MB = size_t{1} << 24;

    static constexpr PhysAddr UNALIGNED_OFFSET = 11;

    std::mt19937_64 mt{MT_SEED};

    PhysMemory pm{};

    // Add test RAM pages
    PhysMemoryTest() {
        for (PhysAddr page_pa = RAM_BASE_PA, end = RAM_BASE_PA + RAM_SIZE_16MB;
             page_pa != end; page_pa += PAGE_SIZE) {

            SIM_ASSERT(pm.addRAMPage(page_pa));
        }
    }
};

} // namespace

// Test valid reads/writes
TEST_F(PhysMemoryTest, readWrite) {
    // [PhysAddr -> Test value]
    std::unordered_map<PhysAddr, uint64_t> test_cases = {
        {RAM_BASE_PA, mt()},
        {RAM_BASE_PA + UNALIGNED_OFFSET, mt()},
        {RAM_BASE_PA + PAGE_SIZE, mt()},
        {RAM_BASE_PA + PAGE_SIZE + UNALIGNED_OFFSET, mt()},
        {RAM_BASE_PA + RAM_SIZE_16MB - sizeof(uint64_t), mt()}};

    // Write test data
    for (const auto &test_case : test_cases) {
        auto pa = test_case.first;
        auto value = test_case.second;

        ASSERT_EQ(pm.write(pa, value).status, Status::OK) << pa;
    }

    // Check written values
    for (const auto &test_case : test_cases) {
        PhysAddr pa = test_case.first;
        uint64_t value = test_case.second;

        uint64_t read_64 = 0;
        ASSERT_EQ(pm.read(pa, read_64).status, Status::OK) << pa;
        ASSERT_EQ(read_64, value) << pa;

        uint32_t read_32 = 0;
        ASSERT_EQ(pm.read(pa, read_32).status, Status::OK) << pa;
        ASSERT_EQ(read_32, static_cast<uint32_t>(value)) << pa;

        uint16_t read_16 = 0;
        ASSERT_EQ(pm.read(pa, read_16).status, Status::OK) << pa;
        ASSERT_EQ(read_16, static_cast<uint16_t>(value)) << pa;

        uint8_t read_8 = 0;
        ASSERT_EQ(pm.read(pa, read_8).status, Status::OK) << pa;
        ASSERT_EQ(read_8, static_cast<uint8_t>(value)) << pa;
    }
}

namespace {} // namespace

// Test reads/writes at unmapped addresses
TEST_F(PhysMemoryTest, rangeError) {
    // [Unmapped PhysAddr]
    const std::vector<PhysAddr> RANGE_ERROR_TEST_CASES = {
        0,
        RAM_BASE_PA - sizeof(uint64_t),
        RAM_BASE_PA + RAM_SIZE_16MB,
        RAM_BASE_PA + RAM_SIZE_16MB + UNALIGNED_OFFSET,
    };

    for (const auto &pa : RANGE_ERROR_TEST_CASES) {
        uint64_t dst = 0;

        ASSERT_EQ(pm.read(pa, dst).status, Status::RANGE_ERROR) << pa;
        ASSERT_EQ(pm.write(pa, uint64_t{}).status, Status::RANGE_ERROR) << pa;
    }
}

// Test page unaligned reads/writes
TEST_F(PhysMemoryTest, pageAlignError) {
    // [Page unaligned access PhysAddr]
    const std::vector<PhysAddr> PAGE_ALIGN_ERROR_TEST_CASES = {
        RAM_BASE_PA - 4, RAM_BASE_PA + PAGE_SIZE - 4,
        RAM_BASE_PA + RAM_SIZE_16MB - 4};

    for (const auto &pa : PAGE_ALIGN_ERROR_TEST_CASES) {
        uint64_t dst = 0;

        ASSERT_EQ(pm.read(pa, dst).status, Status::PAGE_ALIGN_ERROR) << pa;
        ASSERT_EQ(pm.write(pa, uint64_t{}).status, Status::PAGE_ALIGN_ERROR)
            << pa;
    }
}

// Test const host pointers forwarding
TEST_F(PhysMemoryTest, constHostPtr) {
    PhysAddr pa = RAM_BASE_PA + PAGE_SIZE;

    // Check first host page pointer
    uint8_t byte = 0;
    auto [status1, host_page_ptr1] = pm.read(pa, byte);
    ASSERT_EQ(status1, Status::OK);
    ASSERT_TRUE(host_page_ptr1 != nullptr);
    ASSERT_EQ(host_page_ptr1[0], byte);

    // Check second host page pointer
    PhysAddr offset = 0x0ff;
    auto [status2, host_page_ptr2] = pm.read(pa + offset, byte);
    ASSERT_EQ(status2, Status::OK);
    ASSERT_TRUE(host_page_ptr2 != nullptr);
    ASSERT_EQ(host_page_ptr2[offset], byte);

    // Same phys pages => same host pages
    ASSERT_EQ(host_page_ptr1, host_page_ptr2);
}

// Test host pointers forwarding
TEST_F(PhysMemoryTest, hostPtr) {
    PhysAddr pa = RAM_BASE_PA + PAGE_SIZE;

    // Check first host page pointer
    uint8_t byte = mt();
    auto [status1, host_page_ptr1] = pm.write(pa, byte);
    ASSERT_EQ(status1, Status::OK);
    ASSERT_TRUE(host_page_ptr1 != nullptr);
    ASSERT_EQ(host_page_ptr1[0], byte);

    // Check second host page pointer
    PhysAddr offset = 0x0ff;
    byte = mt();
    auto [status2, host_page_ptr2] = pm.write(pa + offset, byte);
    ASSERT_EQ(status2, Status::OK);
    ASSERT_TRUE(host_page_ptr2 != nullptr);
    ASSERT_EQ(host_page_ptr2[offset], byte);

    // Same phys pages => same host pages
    ASSERT_EQ(host_page_ptr1, host_page_ptr2);
}

// Test host pointers coherency
TEST_F(PhysMemoryTest, hostPtrCoherency) {
    PhysAddr pa = RAM_BASE_PA + PAGE_SIZE;

    // Get host ptr
    uint8_t byte = mt();
    auto [status, host_page_ptr] = pm.write(pa, byte);
    ASSERT_EQ(status, Status::OK);
    ASSERT_TRUE(host_page_ptr != nullptr);
    ASSERT_EQ(host_page_ptr[0], byte);

    std::array<uint8_t, PAGE_SIZE> test_data{};

    // Check pm writes coherency
    std::for_each(test_data.begin(), test_data.end(),
                  [this](uint8_t &data) { data = mt(); });
    for (size_t i = 0; i != PAGE_SIZE; ++i) {
        ASSERT_EQ(pm.write(pa + i, test_data[i]).status, Status::OK) << i;
        ASSERT_EQ(test_data[i], host_page_ptr[i]) << i;
    }

    // Check host writes coherency
    std::for_each(test_data.begin(), test_data.end(),
                  [this](uint8_t &data) { data = mt(); });
    for (size_t i = 0; i != PAGE_SIZE; ++i) {
        host_page_ptr[i] = test_data[i];

        ASSERT_EQ(pm.read(pa + i, byte).status, Status::OK) << i;
        ASSERT_EQ(byte, host_page_ptr[i]) << i;
    }
}

} // namespace memory
} // namespace sim
