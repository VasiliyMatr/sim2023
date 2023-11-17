#include <gtest/gtest.h>

#include <iostream>
#include <sim/common.hpp>
#include <sim/instr.hpp>
#include <tuple>

namespace sim {
namespace instr {

TEST(Instr, undef) {
    Instr test = Instr(static_cast<InstrCode>(0));
    ASSERT_EQ(test.id(), InstrId::UNDEF);
}

TEST(Instr, add) {
    Instr test = Instr(static_cast<InstrCode>(0x00710a13)); // addi x20, x2, 7
    ASSERT_EQ(test.id(), InstrId::ADDI);
    ASSERT_EQ(test.rd(), 20);
    ASSERT_EQ(test.rs1(), 2);
    ASSERT_EQ(test.imm(), 7);
}

TEST(Instr, srli) {
    Instr test = Instr(static_cast<InstrCode>(0x02175113)); // srli x2, x14, 33
    ASSERT_EQ(test.id(), InstrId::SRLI);
    ASSERT_EQ(test.rd(), 2);
    ASSERT_EQ(test.rs1(), 14);
    ASSERT_EQ(test.imm(), 33);
}

TEST(Instr, jal) {
    Instr test = Instr(static_cast<InstrCode>(0x02200a6f)); // jal x20, 34
    ASSERT_EQ(test.id(), InstrId::JAL);
    ASSERT_EQ(test.rd(), 20);
    ASSERT_EQ(test.imm(), 34);
}

TEST(Instr, lui) {
    Instr test = Instr(static_cast<InstrCode>(0x0157a2b7)); // lui x5, 5498
    ASSERT_EQ(test.id(), InstrId::LUI);
    ASSERT_EQ(test.rd(), 5);
    ASSERT_EQ(test.imm(), 5498 << 12);
}

TEST(Instr, beq) {
    Instr test = Instr(static_cast<InstrCode>(0x01e18563)); // beq x3, x30, 10
    ASSERT_EQ(test.id(), InstrId::BEQ);
    ASSERT_EQ(test.rs1(), 3);
    ASSERT_EQ(test.rs2(), 30);
    ASSERT_EQ(test.imm(), 10);
}

TEST(Instr, beq_minus) {
    Instr test = Instr(static_cast<InstrCode>(0xffe18fe3)); // beq x3, x30, -2
    ASSERT_EQ(test.id(), InstrId::BEQ);
    ASSERT_EQ(test.rs1(), 3);
    ASSERT_EQ(test.rs2(), 30);
    ASSERT_EQ(test.imm(), static_cast<uint32_t>(-2));
}

TEST(Instr, sraiw) {
    Instr test = Instr(static_cast<InstrCode>(0x41e0d31b)); // sraiw x6, x1, 30
    ASSERT_EQ(test.id(), InstrId::SRAIW);
    ASSERT_EQ(test.rd(), 6);
    ASSERT_EQ(test.rs1(), 1);
    ASSERT_EQ(test.imm(), 30);
}

TEST(Instr, garbage) {
    // Decoder must not crash on any input
    for (uint64_t code = 0, last = std::numeric_limits<InstrCode>::max();
         code <= last; ++code) {
        std::ignore = Instr(static_cast<InstrCode>(code));
    }
}

} // namespace instr
} // namespace sim
