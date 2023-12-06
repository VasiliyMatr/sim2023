#ifndef INCL_SIMULATOR_SIM_INSTR_HPP
#define INCL_SIMULATOR_SIM_INSTR_HPP

#include <sim/simulator.hpp>

namespace sim {

inline constexpr const auto *getSimInstrPtr(instr::InstrId id);

static constexpr VirtAddr PC_ALIGN_MASK = 0x3;

#define SIM_INSTR(INSTR_NAME)                                                  \
    template <>                                                                \
    inline SimStatus Simulator::simInstr<instr::InstrId::INSTR_NAME>(          \
        [[maybe_unused]] Simulator & sim,                                      \
        [[maybe_unused]] const instr::Instr *instr) noexcept

#define SIM_NEXT()                                                             \
    ++instr;                                                                   \
    return sim.dispatch(instr->id())(sim, instr);

#define INCR_AND_SIM_NEXT()                                                    \
    ++sim.m_icount;                                                            \
    sim.m_hart.pc() += INSTR_CODE_SIZE;                                        \
    SIM_NEXT();

SIM_INSTR(SIM_STATUS_INSTR) { return instr->status(); }

SIM_INSTR(ECALL) {
    ++sim.m_icount;
    sim.m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::SIM__EXIT;
}

SIM_INSTR(ADDI) {
    auto &gpr = sim.m_hart.gprFile();

    uint64_t imm = static_cast<int32_t>(instr->imm());
    auto res = gpr.read<int64_t>(instr->rs1()) + imm;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SLTI) {
    auto &gpr = sim.m_hart.gprFile();

    int64_t imm = static_cast<int32_t>(instr->imm());
    uint64_t res = 1 ? gpr.read<int64_t>(instr->rs1()) < imm : 0;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SLTIU) {
    auto &gpr = sim.m_hart.gprFile();

    uint64_t imm = static_cast<int32_t>(instr->imm());
    uint64_t res = 1 ? gpr.read<uint64_t>(instr->rs1()) < imm : 0;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(ANDI) {
    auto &gpr = sim.m_hart.gprFile();

    uint64_t imm = static_cast<int32_t>(instr->imm());
    uint64_t res = gpr.read<uint64_t>(instr->rs1()) & imm;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(ORI) {
    auto &gpr = sim.m_hart.gprFile();

    uint64_t imm = static_cast<int32_t>(instr->imm());
    uint64_t res = gpr.read<uint64_t>(instr->rs1()) | imm;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(XORI) {
    auto &gpr = sim.m_hart.gprFile();

    uint64_t imm = static_cast<int32_t>(instr->imm());
    uint64_t res = gpr.read<uint64_t>(instr->rs1()) ^ imm;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(ADDIW) {
    auto &gpr = sim.m_hart.gprFile();
    auto word_res = instr->imm() + gpr.read<uint32_t>(instr->rs1());

    gpr.write(instr->rd(), static_cast<int32_t>(word_res));

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SLLI) {
    auto &gpr = sim.m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr->rs1()) << instr->imm();

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRLI) {
    auto &gpr = sim.m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr->rs1()) >> instr->imm();

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRAI) {
    auto &gpr = sim.m_hart.gprFile();
    auto res = gpr.read<int64_t>(instr->rs1()) >> instr->imm();

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SLLIW) {
    auto &gpr = sim.m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr->rs1()) << instr->imm();

    gpr.write(instr->rd(), static_cast<int32_t>(word_res));

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRLIW) {
    auto &gpr = sim.m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr->rs1()) >> instr->imm();

    gpr.write(instr->rd(), static_cast<int32_t>(word_res));

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRAIW) {
    auto &gpr = sim.m_hart.gprFile();
    auto word_res = gpr.read<int32_t>(instr->rs1()) >> instr->imm();

    gpr.write(instr->rd(), word_res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(LUI) {
    auto &gpr = sim.m_hart.gprFile();

    gpr.write(instr->rd(), static_cast<int32_t>(instr->imm()));

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(AUIPC) {
    auto &gpr = sim.m_hart.gprFile();
    auto res = static_cast<int64_t>(instr->imm()) + sim.m_hart.pc();

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SLL) {
    auto &gpr = sim.m_hart.gprFile();
    // auto shift = bit::maskBits<uint8_t, 5,
    // 0>(gpr.read<uint8_t>(instr->rs2()));
    auto shift = bit::maskBits(5, 0, gpr.read<uint8_t>(instr->rs2()));
    auto res = gpr.read<uint64_t>(instr->rs1()) << shift;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRL) {
    auto &gpr = sim.m_hart.gprFile();
    auto shift = bit::maskBits(5, 0, gpr.read<uint8_t>(instr->rs2()));
    auto res = gpr.read<uint64_t>(instr->rs1()) >> shift;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRA) {
    auto &gpr = sim.m_hart.gprFile();
    auto shift = bit::maskBits(5, 0, gpr.read<uint8_t>(instr->rs2()));
    auto res = gpr.read<int64_t>(instr->rs1()) >> shift;

    gpr.write(instr->rd(), res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(ADDW) {
    auto &gpr = sim.m_hart.gprFile();
    auto word_res =
        gpr.read<int32_t>(instr->rs1()) + gpr.read<int32_t>(instr->rs2());

    gpr.write(instr->rd(), word_res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SUBW) {
    auto &gpr = sim.m_hart.gprFile();
    auto word_res =
        gpr.read<int32_t>(instr->rs1()) - gpr.read<int32_t>(instr->rs2());

    gpr.write(instr->rd(), word_res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SLLW) {
    auto &gpr = sim.m_hart.gprFile();
    auto shift = bit::maskBits(4, 0, gpr.read<uint8_t>(instr->rs2()));
    auto word_res = gpr.read<uint32_t>(instr->rs1()) << shift;

    gpr.write(instr->rd(), static_cast<int32_t>(word_res));

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRLW) {
    auto &gpr = sim.m_hart.gprFile();
    auto shift = bit::maskBits(4, 0, gpr.read<uint8_t>(instr->rs2()));
    auto word_res = gpr.read<uint32_t>(instr->rs1()) >> shift;

    gpr.write(instr->rd(), static_cast<int32_t>(word_res));

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(SRAW) {
    auto &gpr = sim.m_hart.gprFile();
    auto shift = bit::maskBits(4, 0, gpr.read<uint8_t>(instr->rs2()));
    auto word_res = gpr.read<int32_t>(instr->rs1()) >> shift;

    gpr.write(instr->rd(), word_res);

    INCR_AND_SIM_NEXT();
}

SIM_INSTR(LD) {
    auto status = sim.simLoadInstr<int64_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(LW) {
    auto status = sim.simLoadInstr<int32_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(LH) {
    auto status = sim.simLoadInstr<int16_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(LB) {
    auto status = sim.simLoadInstr<int8_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(LWU) {
    auto status = sim.simLoadInstr<uint32_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(LHU) {
    auto status = sim.simLoadInstr<uint16_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(LBU) {
    auto status = sim.simLoadInstr<uint8_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(SD) {
    auto status = sim.simStoreInstr<uint64_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(SW) {
    auto status = sim.simStoreInstr<uint32_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(SH) {
    auto status = sim.simStoreInstr<uint16_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(SB) {
    auto status = sim.simStoreInstr<uint8_t>(instr);
    if (status != SimStatus::OK) {
        return status;
    }

    SIM_NEXT();
}

SIM_INSTR(JAL) {
    auto &gpr = sim.m_hart.gprFile();

    auto link_pc = sim.m_hart.pc() + 4;
    int64_t offset = static_cast<int32_t>(instr->imm());
    auto new_pc = sim.m_hart.pc() + offset;

    if (new_pc & PC_ALIGN_MASK) {
        return SimStatus::SIM__PC_ALIGN_ERROR;
    }

    gpr.write(instr->rd(), link_pc);

    ++sim.m_icount;
    sim.m_hart.pc() = new_pc;
    return SimStatus::OK;
}

SIM_INSTR(JALR) {
    auto &gpr = sim.m_hart.gprFile();

    auto link_pc = sim.m_hart.pc() + 4;
    int64_t offset = static_cast<int32_t>(instr->imm());
    auto new_pc = (offset + gpr.read<int64_t>(instr->rs1())) ^ 1;

    if (new_pc & PC_ALIGN_MASK) {
        return SimStatus::SIM__PC_ALIGN_ERROR;
    }

    gpr.write(instr->rd(), link_pc);

    ++sim.m_icount;
    sim.m_hart.pc() = new_pc;
    return SimStatus::OK;
}

SIM_INSTR(BEQ) { return sim.simCondBranch<int64_t, std::equal_to>(instr); }

SIM_INSTR(BNE) { return sim.simCondBranch<int64_t, std::not_equal_to>(instr); }

SIM_INSTR(BLT) { return sim.simCondBranch<int64_t, std::less>(instr); }

SIM_INSTR(BLTU) { return sim.simCondBranch<uint64_t, std::less>(instr); }

SIM_INSTR(BGE) { return sim.simCondBranch<int64_t, std::greater_equal>(instr); }

SIM_INSTR(BGEU) {
    return sim.simCondBranch<uint64_t, std::greater_equal>(instr);
}

#undef SIM_INSTR

} // namespace sim

#include <sim/simulator/dispatch.gen.hpp>

#endif // INCL_SIMULATOR_SIM_INSTR_HPP
