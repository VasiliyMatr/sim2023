#ifndef SIM_INSTR_HPP
#define SIM_INSTR_HPP

#include <sim/common.hpp>
#include <sim/instr.hpp>
#include <sim/instr/instr_id.gen.hpp>
#include <sim/simulator.hpp>

namespace sim {

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::ADDIW>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto word_res = instr.imm() + gpr.read<uint32_t>(instr.rs1());
    auto res = bit::signExtend<uint64_t, 31>(word_res);

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::SLLI>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr.rs1()) << instr.imm();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::SRLI>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto res = gpr.read<uint64_t>(instr.rs1()) >> instr.imm();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::SRAI>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto signed_rs_value =
        static_cast<int64_t>(gpr.read<uint64_t>(instr.rs1()));
    auto res = signed_rs_value >> instr.imm();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::SLLIW>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr.rs1()) << instr.imm();
    auto res = bit::signExtend<uint64_t, 31>(word_res);

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::SRLIW>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto word_res = gpr.read<uint32_t>(instr.rs1()) >> instr.imm();
    auto res = bit::signExtend<uint64_t, 31>(word_res);

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::SRAIW>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto signed_rs_value =
        static_cast<int32_t>(gpr.read<uint32_t>(instr.rs1()));
    auto word_res = signed_rs_value >> instr.imm();
    auto res = bit::signExtend<uint64_t, 31>(word_res);

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::LUI>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto res =  bit::signExtend<uint64_t, 31>(instr.imm());

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}


template <>
Simulator::SimStatus
Simulator::simInstr<instr::InstrId::AUIPC>(const instr::Instr &instr) noexcept {
    auto &gpr = m_hart.gprFile();
    auto imm =  bit::signExtend<uint64_t, 31>(instr.imm());
    auto res = imm + m_hart.pc();

    gpr.write(instr.rd(), res);

    return SimStatus::OK;
}

} // namespace sim

#endif // SIM_INSTR_HPP
