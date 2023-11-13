#ifndef INCL_SIM_INSTR_HPP
#define INCL_SIM_INSTR_HPP

#include "sim/hart.hpp"
#include "sim/memory.hpp"
#include <cstdint>

#include <sim/common.hpp>
#include <sim/instr.hpp>
#include <sim/instr/instr_id.gen.hpp>
#include <sim/simulator.hpp>

namespace sim {

static constexpr size_t INSTR_CODE_SIZE = sizeof(InstrCode);
static constexpr PhysAddr PC_ALIGN_MASK = 0x3;

#define SIM_INSTR(INSTR_NAME)                                                  \
    template <>                                                                \
    Simulator::SimStatus Simulator::simInstr<instr::InstrId::INSTR_NAME>(      \
        const instr::Instr &instr) noexcept

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
    // auto shift = bit::maskBits<uint8_t, 5, 0>(gpr.read<uint8_t>(instr.rs2()));
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

SIM_INSTR(LD) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    uint64_t res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LW) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    int32_t word_res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, word_res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), word_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LWU) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    uint32_t word_res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, word_res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), word_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LH) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    int16_t half_res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, half_res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), half_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LHU) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    uint16_t half_res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, half_res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), half_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LB) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    int8_t byte_res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, byte_res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), byte_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(LBU) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    uint8_t byte_res = 0;
    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());

    auto status = phys_memory.read(phys_addr, byte_res);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    gpr.write(instr.rd(), byte_res);

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SD) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint64_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SW) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint32_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SH) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint16_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(SB) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint8_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);
    if (status != memory::PhysMemory::AccessStatus::OK) {
        return SimStatus::PHYS_MEMORY_ERROR;
    }

    m_hart.pc() += INSTR_CODE_SIZE;
    return SimStatus::OK;
}

SIM_INSTR(JAL) {
    auto &gpr = m_hart.gprFile();

    auto link_pc = m_hart.pc() + 4;
    auto offset = static_cast<int64_t>(instr.imm()) << 1;
    auto new_pc = m_hart.pc() + offset;

    if (new_pc & PC_ALIGN_MASK) {
        return SimStatus::PC_ALIGN_ERROR;
    }

    gpr.write(instr.rd(), link_pc);

    m_hart.pc() = new_pc;
    return SimStatus::OK;
}

SIM_INSTR(JALR) {
    auto &gpr = m_hart.gprFile();

    auto link_pc = m_hart.pc() + 4;
    int64_t offset = instr.imm();
    auto new_pc = (offset + gpr.read<int64_t>(instr.rs1())) ^ 1;

    if (new_pc & PC_ALIGN_MASK) {
        return SimStatus::PC_ALIGN_ERROR;
    }

    gpr.write(instr.rd(), link_pc);

    m_hart.pc() = new_pc;
    return SimStatus::OK;
}

namespace {

Simulator::SimStatus simCondBranch(const instr::Instr &instr, hart::Hart &hart,
                                   bool cond) {
    if (cond) {
        auto offset = static_cast<int64_t>(instr.imm()) << 1;
        auto new_pc = hart.pc() + offset;

        if (new_pc & PC_ALIGN_MASK) {
            return Simulator::SimStatus::PC_ALIGN_ERROR;
        }

        hart.pc() = new_pc;
        return Simulator::SimStatus::OK;
    }

    hart.pc() += INSTR_CODE_SIZE;
    return Simulator::SimStatus::OK;
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

#endif // INCL_SIM_INSTR_HPP
