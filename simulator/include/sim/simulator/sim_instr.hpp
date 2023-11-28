#ifndef INCL_SIMULATOR_SIM_INSTR_HPP
#define INCL_SIMULATOR_SIM_INSTR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/instr/instr_id.gen.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>

namespace sim {

static constexpr VirtAddr PC_ALIGN_MASK = 0x3;

#define SIM_INSTR(INSTR_NAME)                                                  \
    template <>                                                                \
    inline SimStatus Simulator::simInstr<instr::InstrId::INSTR_NAME>(          \
        [[maybe_unused]] const instr::Instr &instr) noexcept

SIM_INSTR(ECALL) {
    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::SIM__EXIT;
}

SIM_INSTR(ADDIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = instr.imm() + gpr.read<uint32_t>(instr.rs1());

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SLLI) {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr.rs1()) << instr.imm();

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRLI) {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRAI) {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<int64_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SLLIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr.rs1()) << instr.imm();

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRLIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRAIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<int32_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), word_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LUI) {
    auto &gpr = m_hart.gprFile();

    gpr.write(instr.rd(), static_cast<int32_t>(instr.imm()));

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(AUIPC) {
    auto &gpr = m_hart.gprFile();
    auto res = static_cast<int64_t>(instr.imm()) + m_hart.pc();

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SLL) {
    auto &gpr = m_hart.gprFile();
    // auto shift = bit::maskBits<uint8_t, 5,
    // 0>(gpr.read<uint8_t>(instr.rs2()));
    auto shift = bit::maskBits(5, 0, gpr.read<uint8_t>(instr.rs2()));
    auto res = gpr.read<uint64_t>(instr.rs1()) << shift;

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRL) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits(5, 0, gpr.read<uint8_t>(instr.rs2()));
    auto res = gpr.read<uint64_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRA) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits(5, 0, gpr.read<uint8_t>(instr.rs2()));
    auto res = gpr.read<int64_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(ADDW) {
    auto &gpr = m_hart.gprFile();
    auto word_res =
        gpr.read<int32_t>(instr.rs1()) + gpr.read<int32_t>(instr.rs2());

    gpr.write(instr.rd(), word_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SUBW) {
    auto &gpr = m_hart.gprFile();
    auto word_res =
        gpr.read<int32_t>(instr.rs1()) - gpr.read<int32_t>(instr.rs2());

    gpr.write(instr.rd(), word_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SLLW) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits(4, 0, gpr.read<uint8_t>(instr.rs2()));
    auto word_res = gpr.read<uint32_t>(instr.rs1()) << shift;

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRLW) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits(4, 0, gpr.read<uint8_t>(instr.rs2()));
    auto word_res = gpr.read<uint32_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SRAW) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits(4, 0, gpr.read<uint8_t>(instr.rs2()));
    auto word_res = gpr.read<int32_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), word_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LD) { return simLoadInstr<int64_t>(instr); }
SIM_INSTR(LW) { return simLoadInstr<int32_t>(instr); }
SIM_INSTR(LH) { return simLoadInstr<int16_t>(instr); }
SIM_INSTR(LB) { return simLoadInstr<int8_t>(instr); }

SIM_INSTR(LWU) { return simLoadInstr<uint32_t>(instr); }
SIM_INSTR(LHU) { return simLoadInstr<uint16_t>(instr); }
SIM_INSTR(LBU) { return simLoadInstr<uint8_t>(instr); }

SIM_INSTR(SD) { return simStoreInstr<uint64_t>(instr); }
SIM_INSTR(SW) { return simStoreInstr<uint32_t>(instr); }
SIM_INSTR(SH) { return simStoreInstr<uint16_t>(instr); }
SIM_INSTR(SB) { return simStoreInstr<uint8_t>(instr); }

SIM_INSTR(JAL) {
    auto &gpr = m_hart.gprFile();

    auto link_pc = m_hart.pc() + 4;
    int64_t offset = static_cast<int32_t>(instr.imm());
    auto new_pc = m_hart.pc() + offset;

    if (new_pc & PC_ALIGN_MASK) {
        return SimStatus::SIM__PC_ALIGN_ERROR;
    }

    gpr.write(instr.rd(), link_pc);

    m_hart.pc() = new_pc;
    return SimStatus::OK;
}

SIM_INSTR(JALR) {
    auto &gpr = m_hart.gprFile();

    auto link_pc = m_hart.pc() + 4;
    int64_t offset = static_cast<int32_t>(instr.imm());
    auto new_pc = (offset + gpr.read<int64_t>(instr.rs1())) ^ 1;

    if (new_pc & PC_ALIGN_MASK) {
        return SimStatus::SIM__PC_ALIGN_ERROR;
    }

    gpr.write(instr.rd(), link_pc);

    m_hart.pc() = new_pc;
    return SimStatus::OK;
}

namespace {

SimStatus simCondBranch(const instr::Instr &instr, hart::Hart &hart,
                        bool cond) {
    if (cond) {
        auto offset = static_cast<int64_t>(instr.imm());
        auto new_pc = hart.pc() + offset;

        if (new_pc & PC_ALIGN_MASK) {
            return SimStatus::SIM__PC_ALIGN_ERROR;
        }

        hart.pc() = new_pc;
        return SimStatus::OK;
    }

    hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

} // namespace

SIM_INSTR(BEQ) {
    auto &gpr = m_hart.gprFile();

    auto rs1 = gpr.read<int64_t>(instr.rs1());
    auto rs2 = gpr.read<int64_t>(instr.rs2());

    return simCondBranch(instr, m_hart, rs1 == rs2);
}

SIM_INSTR(BNE) {
    auto &gpr = m_hart.gprFile();

    auto rs1 = gpr.read<int64_t>(instr.rs1());
    auto rs2 = gpr.read<int64_t>(instr.rs2());

    return simCondBranch(instr, m_hart, rs1 != rs2);
}

SIM_INSTR(BLT) {
    auto &gpr = m_hart.gprFile();

    auto rs1 = gpr.read<int64_t>(instr.rs1());
    auto rs2 = gpr.read<int64_t>(instr.rs2());

    return simCondBranch(instr, m_hart, rs1 < rs2);
}

SIM_INSTR(BLTU) {
    auto &gpr = m_hart.gprFile();

    auto rs1 = gpr.read<uint64_t>(instr.rs1());
    auto rs2 = gpr.read<uint64_t>(instr.rs2());

    return simCondBranch(instr, m_hart, rs1 < rs2);
}

SIM_INSTR(BGE) {
    auto &gpr = m_hart.gprFile();

    auto rs1 = gpr.read<int64_t>(instr.rs1());
    auto rs2 = gpr.read<int64_t>(instr.rs2());

    return simCondBranch(instr, m_hart, rs1 >= rs2);
}

SIM_INSTR(BGEU) {
    auto &gpr = m_hart.gprFile();

    auto rs1 = gpr.read<uint64_t>(instr.rs1());
    auto rs2 = gpr.read<uint64_t>(instr.rs2());

    return simCondBranch(instr, m_hart, rs1 >= rs2);
}

#undef SIM_INSTR

} // namespace sim

#endif // INCL_SIMULATOR_SIM_INSTR_HPP
