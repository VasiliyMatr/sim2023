#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>

#include <sim/simulator/sim_instr.hpp>

namespace sim {

SimStatus Simulator::simulate(VirtAddr start_pc) {
    m_hart.pc() = start_pc;
    m_icount = 0;

    bb::Bb bb{};

    while (true) {
        // Fetch & decode bb
        auto fetch = Fetch(m_hart.pc(), *this);
        bb.update(m_hart.pc(), fetch);

        // Execute
        const auto *instrs = bb.instrs();
        auto status = dispatch(instrs->id())(*this, instrs);

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
