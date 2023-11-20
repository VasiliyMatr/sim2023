
#include <sim/common.hpp>
#include <sim/instr.hpp>
#include <sim/hart.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>
#include <sim/simulator/sim_instr.hpp>

namespace sim {

Simulator::SimStatus Simulator::simulate(PhysAddr start_pc) {

    while (start_pc < m_phys_memory.size() * 4) {
        // Fetch
        InstrCode instr_code = 0;
        memory::PhysMemory::AccessStatus mem_status = m_phys_memory.read<InstrCode>(start_pc, instr_code); // fetched
        if (mem_status == memory::PhysMemory::AccessStatus::RANGE_ERROR) {
            return SimStatus::ERROR;
        }

        // Create instr from intrcode == Decode
        instr::Instr instruction{instr_code};

        // Execute
        Simulator::SimStatus status;
        switch(instruction.id()){
            case instr::InstrId::ADDIW:
                status = Simulator::simInstr<instr::InstrId::ADDIW>(instruction);
                break;
            case instr::InstrId::SLLI:
                status = Simulator::simInstr<instr::InstrId::SLLI>(instruction);
                break;
            case instr::InstrId::SRLI:
                status = Simulator::simInstr<instr::InstrId::SRLI>(instruction);
                break;
            case instr::InstrId::SRAI:
                status = Simulator::simInstr<instr::InstrId::SRAI>(instruction);
                break;
            case instr::InstrId::SLLIW:
                status = Simulator::simInstr<instr::InstrId::SLLIW>(instruction);
                break;
            case instr::InstrId::SRLIW:
                status = Simulator::simInstr<instr::InstrId::SRLIW>(instruction);
                break;
            case instr::InstrId::SRAIW:
                status = Simulator::simInstr<instr::InstrId::SRAIW>(instruction);
                break;
            case instr::InstrId::LUI:
                status = Simulator::simInstr<instr::InstrId::LUI>(instruction);
                break;
            case instr::InstrId::AUIPC:
                status = Simulator::simInstr<instr::InstrId::AUIPC>(instruction);
                break;
            case instr::InstrId::SLL:
                status = Simulator::simInstr<instr::InstrId::SLL>(instruction);
                break;
            case instr::InstrId::SRL:
                status = Simulator::simInstr<instr::InstrId::SRL>(instruction);
                break;
            case instr::InstrId::SRA:
                status = Simulator::simInstr<instr::InstrId::SRA>(instruction);
                break;
            case instr::InstrId::ADDW:
                status = Simulator::simInstr<instr::InstrId::ADDW>(instruction);
                break;
            case instr::InstrId::SUBW:
                status = Simulator::simInstr<instr::InstrId::SUBW>(instruction);
                break;
            case instr::InstrId::SLLW:
                status = Simulator::simInstr<instr::InstrId::SLLW>(instruction);
                break;
            case instr::InstrId::SRLW:
                status = Simulator::simInstr<instr::InstrId::SRLW>(instruction);
                break;
            case instr::InstrId::SRAW:
                status = Simulator::simInstr<instr::InstrId::SRAW>(instruction);
                break;
            case instr::InstrId::LD:
                status = Simulator::simInstr<instr::InstrId::LD>(instruction);
                break;
            case instr::InstrId::LW:
                status = Simulator::simInstr<instr::InstrId::LW>(instruction);
                break;
            case instr::InstrId::LWU:
                status = Simulator::simInstr<instr::InstrId::LWU>(instruction);
                break;
            case instr::InstrId::LH:
                status = Simulator::simInstr<instr::InstrId::LH>(instruction);
                break;
            case instr::InstrId::LHU:
                status = Simulator::simInstr<instr::InstrId::LHU>(instruction);
                break;
            case instr::InstrId::LB:
                status = Simulator::simInstr<instr::InstrId::LB>(instruction);
                break;
            case instr::InstrId::LBU:
                status = Simulator::simInstr<instr::InstrId::LBU>(instruction);
                break;
            case instr::InstrId::SD:
                status = Simulator::simInstr<instr::InstrId::SD>(instruction);
                break;
            case instr::InstrId::SW:
                status = Simulator::simInstr<instr::InstrId::SW>(instruction);
                break;
            case instr::InstrId::SH:
                status = Simulator::simInstr<instr::InstrId::SH>(instruction);
                break;
            case instr::InstrId::SB:
                status = Simulator::simInstr<instr::InstrId::SB>(instruction);
                break;
            case instr::InstrId::JAL:
                status = Simulator::simInstr<instr::InstrId::JAL>(instruction);
                break;
            case instr::InstrId::JALR:
                status = Simulator::simInstr<instr::InstrId::JALR>(instruction);
                break;
            case instr::InstrId::BEQ:
                status = Simulator::simInstr<instr::InstrId::BEQ>(instruction);
                break;
            case instr::InstrId::BNE:
                status = Simulator::simInstr<instr::InstrId::BNE>(instruction);
                break;
            case instr::InstrId::BLT:
                status = Simulator::simInstr<instr::InstrId::BLT>(instruction);
                break;
            case instr::InstrId::BLTU:
                status = Simulator::simInstr<instr::InstrId::BLTU>(instruction);
                break;
            case instr::InstrId::BGE:
                status = Simulator::simInstr<instr::InstrId::BGE>(instruction);
                break;
            case instr::InstrId::BGEU:
                status = Simulator::simInstr<instr::InstrId::BGEU>(instruction);
               break;
            default:
                return SimStatus::NOT_IMPLEMENTED_INSTR;
            }
        start_pc += 4;
    }
    return SimStatus::OK;
}

} // namespace sim