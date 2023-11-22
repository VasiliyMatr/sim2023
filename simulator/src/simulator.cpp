
#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>
#include <sim/simulator/sim_instr.hpp>

namespace sim {

hart::Hart &Simulator::getHart() { return m_hart; }

memory::PhysMemory &Simulator::getPhysMemory() { return m_phys_memory; }

void Simulator::loadToMemory(const std::vector<InstrCode> &instructions,
                             PhysAddr start_addr) {

    SIM_ASSERT(m_phys_memory.size() != 0 && !instructions.empty());
    SIM_ASSERT(instructions.size() >= instructions.size());

    memory::PhysMemory::AccessStatus status;
    for (size_t index = 0; index < instructions.size(); index++) {
        status = m_phys_memory.write(index + start_addr, instructions[index]);
        if (status == memory::PhysMemory::AccessStatus::RANGE_ERROR) {
            return; // TODO: add normal error
        }
    }
}

Simulator::SimStatus Simulator::simulate(PhysAddr start_pc) {
    m_hart.pc() = start_pc;
    while (true) {
        // Fetch
        InstrCode instr_code = 0;
        memory::PhysMemory::AccessStatus mem_status =
            m_phys_memory.read<InstrCode>(start_pc, instr_code); // fetched
        if (mem_status == memory::PhysMemory::AccessStatus::RANGE_ERROR) {
            return SimStatus::PHYS_MEMORY_ERROR;
        }

        // Decode
        instr::Instr instruction{instr_code};

        // Execute
        SimStatus status;
        switch (instruction.id()) {
        case instr::InstrId::ADDIW:
            status = simInstr<instr::InstrId::ADDIW>(instruction);
            break;
        case instr::InstrId::SLLI:
            status = simInstr<instr::InstrId::SLLI>(instruction);
            break;
        case instr::InstrId::SRLI:
            status = simInstr<instr::InstrId::SRLI>(instruction);
            break;
        case instr::InstrId::SRAI:
            status = simInstr<instr::InstrId::SRAI>(instruction);
            break;
        case instr::InstrId::SLLIW:
            status = simInstr<instr::InstrId::SLLIW>(instruction);
            break;
        case instr::InstrId::SRLIW:
            status = simInstr<instr::InstrId::SRLIW>(instruction);
            break;
        case instr::InstrId::SRAIW:
            status = simInstr<instr::InstrId::SRAIW>(instruction);
            break;
        case instr::InstrId::LUI:
            status = simInstr<instr::InstrId::LUI>(instruction);
            break;
        case instr::InstrId::AUIPC:
            status = simInstr<instr::InstrId::AUIPC>(instruction);
            break;
        case instr::InstrId::SLL:
            status = simInstr<instr::InstrId::SLL>(instruction);
            break;
        case instr::InstrId::SRL:
            status = simInstr<instr::InstrId::SRL>(instruction);
            break;
        case instr::InstrId::SRA:
            status = simInstr<instr::InstrId::SRA>(instruction);
            break;
        case instr::InstrId::ADDW:
            status = simInstr<instr::InstrId::ADDW>(instruction);
            break;
        case instr::InstrId::SUBW:
            status = simInstr<instr::InstrId::SUBW>(instruction);
            break;
        case instr::InstrId::SLLW:
            status = simInstr<instr::InstrId::SLLW>(instruction);
            break;
        case instr::InstrId::SRLW:
            status = simInstr<instr::InstrId::SRLW>(instruction);
            break;
        case instr::InstrId::SRAW:
            status = simInstr<instr::InstrId::SRAW>(instruction);
            break;
        case instr::InstrId::LD:
            status = simInstr<instr::InstrId::LD>(instruction);
            break;
        case instr::InstrId::LW:
            status = simInstr<instr::InstrId::LW>(instruction);
            break;
        case instr::InstrId::LWU:
            status = simInstr<instr::InstrId::LWU>(instruction);
            break;
        case instr::InstrId::LH:
            status = simInstr<instr::InstrId::LH>(instruction);
            break;
        case instr::InstrId::LHU:
            status = simInstr<instr::InstrId::LHU>(instruction);
            break;
        case instr::InstrId::LB:
            status = simInstr<instr::InstrId::LB>(instruction);
            break;
        case instr::InstrId::LBU:
            status = simInstr<instr::InstrId::LBU>(instruction);
            break;
        case instr::InstrId::SD:
            status = simInstr<instr::InstrId::SD>(instruction);
            break;
        case instr::InstrId::SW:
            status = simInstr<instr::InstrId::SW>(instruction);
            break;
        case instr::InstrId::SH:
            status = simInstr<instr::InstrId::SH>(instruction);
            break;
        case instr::InstrId::SB:
            status = simInstr<instr::InstrId::SB>(instruction);
            break;
        case instr::InstrId::JAL:
            status = simInstr<instr::InstrId::JAL>(instruction);
            break;
        case instr::InstrId::JALR:
            status = simInstr<instr::InstrId::JALR>(instruction);
            break;
        case instr::InstrId::BEQ:
            status = simInstr<instr::InstrId::BEQ>(instruction);
            break;
        case instr::InstrId::BNE:
            status = simInstr<instr::InstrId::BNE>(instruction);
            break;
        case instr::InstrId::BLT:
            status = simInstr<instr::InstrId::BLT>(instruction);
            break;
        case instr::InstrId::BLTU:
            status = simInstr<instr::InstrId::BLTU>(instruction);
            break;
        case instr::InstrId::BGE:
            status = simInstr<instr::InstrId::BGE>(instruction);
            break;
        case instr::InstrId::BGEU:
            status = simInstr<instr::InstrId::BGEU>(instruction);
            break;
        case instr::InstrId::ECALL:
            status = simInstr<instr::InstrId::ECALL>(instruction);
            break;
        default:
            status = SimStatus::NOT_IMPLEMENTED_INSTR;
            break;
        }

        if (status == SimStatus::EXIT) {
            return SimStatus::OK;
        }
        if (status != SimStatus::OK) {
            return status;
        }
        start_pc += sizeof(InstrCode);
        m_hart.pc() = start_pc;
    }
    return SimStatus::OK;
}

} // namespace sim
