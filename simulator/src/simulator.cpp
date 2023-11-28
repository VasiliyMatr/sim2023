#include "sim/instr/instr_id.gen.hpp"
#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>

#include <sim/simulator/sim_instr.hpp>

namespace sim {

SimStatus Simulator::simBb(const bb::Bb &bb) noexcept {
    for (const auto &instr : bb) {
        auto status = SimStatus::SIM__NOT_IMPLEMENTED_INSTR;

        switch (instr.id()) {
        case instr::InstrId::ADDI:
            status = simInstr<instr::InstrId::ADDI>(instr);
            break;
        case instr::InstrId::SLTI:
            status = simInstr<instr::InstrId::SLTI>(instr);
            break;
        case instr::InstrId::SLTIU:
            status = simInstr<instr::InstrId::SLTIU>(instr);
            break;
        case instr::InstrId::ANDI:
            status = simInstr<instr::InstrId::ANDI>(instr);
            break;
        case instr::InstrId::ORI:
            status = simInstr<instr::InstrId::ORI>(instr);
            break;
        case instr::InstrId::XORI:
            status = simInstr<instr::InstrId::XORI>(instr);
            break;
        case instr::InstrId::ADDIW:
            status = simInstr<instr::InstrId::ADDIW>(instr);
            break;
        case instr::InstrId::SLLI:
            status = simInstr<instr::InstrId::SLLI>(instr);
            break;
        case instr::InstrId::SRLI:
            status = simInstr<instr::InstrId::SRLI>(instr);
            break;
        case instr::InstrId::SRAI:
            status = simInstr<instr::InstrId::SRAI>(instr);
            break;
        case instr::InstrId::SLLIW:
            status = simInstr<instr::InstrId::SLLIW>(instr);
            break;
        case instr::InstrId::SRLIW:
            status = simInstr<instr::InstrId::SRLIW>(instr);
            break;
        case instr::InstrId::SRAIW:
            status = simInstr<instr::InstrId::SRAIW>(instr);
            break;
        case instr::InstrId::LUI:
            status = simInstr<instr::InstrId::LUI>(instr);
            break;
        case instr::InstrId::AUIPC:
            status = simInstr<instr::InstrId::AUIPC>(instr);
            break;
        case instr::InstrId::SLL:
            status = simInstr<instr::InstrId::SLL>(instr);
            break;
        case instr::InstrId::SRL:
            status = simInstr<instr::InstrId::SRL>(instr);
            break;
        case instr::InstrId::SRA:
            status = simInstr<instr::InstrId::SRA>(instr);
            break;
        case instr::InstrId::ADDW:
            status = simInstr<instr::InstrId::ADDW>(instr);
            break;
        case instr::InstrId::SUBW:
            status = simInstr<instr::InstrId::SUBW>(instr);
            break;
        case instr::InstrId::SLLW:
            status = simInstr<instr::InstrId::SLLW>(instr);
            break;
        case instr::InstrId::SRLW:
            status = simInstr<instr::InstrId::SRLW>(instr);
            break;
        case instr::InstrId::SRAW:
            status = simInstr<instr::InstrId::SRAW>(instr);
            break;
        case instr::InstrId::LD:
            status = simInstr<instr::InstrId::LD>(instr);
            break;
        case instr::InstrId::LW:
            status = simInstr<instr::InstrId::LW>(instr);
            break;
        case instr::InstrId::LWU:
            status = simInstr<instr::InstrId::LWU>(instr);
            break;
        case instr::InstrId::LH:
            status = simInstr<instr::InstrId::LH>(instr);
            break;
        case instr::InstrId::LHU:
            status = simInstr<instr::InstrId::LHU>(instr);
            break;
        case instr::InstrId::LB:
            status = simInstr<instr::InstrId::LB>(instr);
            break;
        case instr::InstrId::LBU:
            status = simInstr<instr::InstrId::LBU>(instr);
            break;
        case instr::InstrId::SD:
            status = simInstr<instr::InstrId::SD>(instr);
            break;
        case instr::InstrId::SW:
            status = simInstr<instr::InstrId::SW>(instr);
            break;
        case instr::InstrId::SH:
            status = simInstr<instr::InstrId::SH>(instr);
            break;
        case instr::InstrId::SB:
            status = simInstr<instr::InstrId::SB>(instr);
            break;
        case instr::InstrId::JAL:
            status = simInstr<instr::InstrId::JAL>(instr);
            break;
        case instr::InstrId::JALR:
            status = simInstr<instr::InstrId::JALR>(instr);
            break;
        case instr::InstrId::BEQ:
            status = simInstr<instr::InstrId::BEQ>(instr);
            break;
        case instr::InstrId::BNE:
            status = simInstr<instr::InstrId::BNE>(instr);
            break;
        case instr::InstrId::BLT:
            status = simInstr<instr::InstrId::BLT>(instr);
            break;
        case instr::InstrId::BLTU:
            status = simInstr<instr::InstrId::BLTU>(instr);
            break;
        case instr::InstrId::BGE:
            status = simInstr<instr::InstrId::BGE>(instr);
            break;
        case instr::InstrId::BGEU:
            status = simInstr<instr::InstrId::BGEU>(instr);
            break;
        case instr::InstrId::ECALL:
            status = simInstr<instr::InstrId::ECALL>(instr);
            break;
        default:
            status = SimStatus::SIM__NOT_IMPLEMENTED_INSTR;
            break;
        }

        if (status != SimStatus::OK) {
            return status;
        }

        ++m_icount;
    }

    return SimStatus::OK;
}

SimStatus Simulator::simulate(VirtAddr start_pc) {
    m_hart.pc() = start_pc;
    m_icount = 0;

    bb::Bb bb{};

    while (true) {
        // Fetch & decode bb
        auto fetch = Fetch(m_hart.pc(), *this);
        bb.update(m_hart.pc(), fetch);

        // Execute
        auto status = simBb(bb);

        if (status == SimStatus::SIM__EXIT) {
            return SimStatus::OK;
        }

        if (status != SimStatus::OK) {
            return status;
        }
    }

    SIM_ASSERT(0);
}

} // namespace sim
