
#include "gtest/gtest.h"
#include <random>

#include <gtest/gtest.h>

#include <sim/common.hpp>
#include <sim/gpr.hpp>

namespace sim {
namespace gpr {

class GPRTest : public ::testing::Test {
  protected:
    static constexpr uint64_t MT_SEED = 1003;
    std::mt19937_64 mt{MT_SEED};

    GPRFile gpr_file{};
};

TEST_F(GPRTest, zero) {
    auto test_value = mt();

    gpr_file.write(GPR_IDX::ZERO, test_value);
    ASSERT_EQ(gpr_file.read<uint64_t>(GPR_IDX::ZERO), 0);
    ASSERT_EQ(gpr_file.read<uint32_t>(GPR_IDX::ZERO), 0);
    ASSERT_EQ(gpr_file.read<uint16_t>(GPR_IDX::ZERO), 0);
    ASSERT_EQ(gpr_file.read<uint8_t>(GPR_IDX::ZERO), 0);
}

TEST_F(GPRTest, readWrite) {
    for (size_t i = 1; i != GPR_NUMBER; ++i) {
        gpr_file.write(i, mt());
    }

    std::mt19937_64 mt{MT_SEED};
    for (size_t i = 1; i != GPR_NUMBER; ++i) {
        auto value = mt();

        ASSERT_EQ(gpr_file.read<uint64_t>(i), value);
        ASSERT_EQ(gpr_file.read<uint32_t>(i), static_cast<uint32_t>(value));
        ASSERT_EQ(gpr_file.read<uint16_t>(i), static_cast<uint16_t>(value));
        ASSERT_EQ(gpr_file.read<uint8_t>(i), static_cast<uint8_t>(value));
    }
}

TEST_F(GPRTest, outOfRangeRead) {
    const char *expected_msg = "Assertion failed: idx < GPR_NUMBER";

    ASSERT_DEATH(std::ignore = gpr_file.read<uint64_t>(GPR_NUMBER),
                 expected_msg);
    ASSERT_DEATH(std::ignore = gpr_file.read<uint32_t>(GPR_NUMBER),
                 expected_msg);
    ASSERT_DEATH(std::ignore = gpr_file.read<uint16_t>(GPR_NUMBER),
                 expected_msg);
    ASSERT_DEATH(std::ignore = gpr_file.read<uint8_t>(GPR_NUMBER),
                 expected_msg);
}

TEST_F(GPRTest, outOfRangeWrite) {
    const char *expected_msg = "Assertion failed: idx < GPR_NUMBER";

    ASSERT_DEATH(gpr_file.write(GPR_NUMBER, mt()), expected_msg);
}

} // namespace gpr
} // namespace sim