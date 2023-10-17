#ifndef INCL_SIM_INSTR_HPP
#define INCL_SIM_INSTR_HPP

#include "sim/memory.hpp"
#include <cstdint>

#include <sim/common.hpp>
#include <sim/instr.hpp>
#include <sim/instr/instr_id.gen.hpp>
#include <sim/simulator.hpp>

namespace sim {

#define SIM_INSTR(INSTR_NAME)                                                  \
    template <>                                                                \
    Simulator::SimStatus Simulator::simInstr<instr::InstrId::INSTR_NAME>(      \
        const instr::Instr &instr) noexcept

SIM_INSTR(ADDIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = instr.imm() + gpr.read<uint32_t>(instr.rs1());

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    return SimStatus::OK;
}

SIM_INSTR(SLLI) {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr.rs1()) << instr.imm();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(SRLI) {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(SRAI) {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<int64_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(SLLIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr.rs1()) << instr.imm();

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    return SimStatus::OK;
}

SIM_INSTR(SRLIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    return SimStatus::OK;
}

SIM_INSTR(SRAIW) {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<int32_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), word_res);

    return SimStatus::OK;
}

SIM_INSTR(LUI) {
    auto &gpr = m_hart.gprFile();

    gpr.write(instr.rd(), static_cast<int32_t>(instr.imm()));

    return SimStatus::OK;
}

SIM_INSTR(AUIPC) {
    auto &gpr = m_hart.gprFile();
    auto res = static_cast<int64_t>(instr.imm()) + m_hart.pc();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(SLL) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits<uint8_t, 5, 0>(gpr.read<uint8_t>(instr.rs2()));
    auto res = gpr.read<uint64_t>(instr.rs1()) << shift;

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(SRL) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits<uint8_t, 5, 0>(gpr.read<uint8_t>(instr.rs2()));
    auto res = gpr.read<uint64_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(SRA) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits<uint8_t, 5, 0>(gpr.read<uint8_t>(instr.rs2()));
    auto res = gpr.read<int64_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

SIM_INSTR(ADDW) {
    auto &gpr = m_hart.gprFile();
    auto word_res =
        gpr.read<int32_t>(instr.rs1()) + gpr.read<int32_t>(instr.rs2());

    gpr.write(instr.rd(), word_res);

    return SimStatus::OK;
}

SIM_INSTR(SUBW) {
    auto &gpr = m_hart.gprFile();
    auto word_res =
        gpr.read<int32_t>(instr.rs1()) - gpr.read<int32_t>(instr.rs2());

    gpr.write(instr.rd(), word_res);

    return SimStatus::OK;
}

SIM_INSTR(SLLW) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits<uint8_t, 4, 0>(gpr.read<uint8_t>(instr.rs2()));
    auto word_res = gpr.read<uint32_t>(instr.rs1()) << shift;

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    return SimStatus::OK;
}

SIM_INSTR(SRLW) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits<uint8_t, 4, 0>(gpr.read<uint8_t>(instr.rs2()));
    auto word_res = gpr.read<uint32_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), static_cast<int32_t>(word_res));

    return SimStatus::OK;
}

SIM_INSTR(SRAW) {
    auto &gpr = m_hart.gprFile();
    auto shift = bit::maskBits<uint8_t, 4, 0>(gpr.read<uint8_t>(instr.rs2()));
    auto word_res = gpr.read<int32_t>(instr.rs1()) >> shift;

    gpr.write(instr.rd(), word_res);

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

    return SimStatus::OK;
}

SIM_INSTR(SD) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint64_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);

    return SimStatus::OK;
}

SIM_INSTR(SW) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint32_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);

    return SimStatus::OK;
}

SIM_INSTR(SH) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint16_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);

    return SimStatus::OK;
}

SIM_INSTR(SB) {
    auto &gpr = m_hart.gprFile();
    auto &phys_memory = m_hart.physMemory();

    auto phys_addr = gpr.read<PhysAddr>(instr.rs1());
    auto value = gpr.read<uint8_t>(instr.rs2());

    auto status = phys_memory.write(phys_addr, value);

    return SimStatus::OK;
}

#undef SIM_INSTR

} // namespace sim

#endif // INCL_SIM_INSTR_HPP
