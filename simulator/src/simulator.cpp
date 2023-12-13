#include <sim/simulator.hpp>
#include <sim/simulator/sim_instr.hpp>

namespace sim {

SimStatus Simulator::simulate(VirtAddr start_pc) {
    m_hart.pc() = start_pc;
    m_icount = 0;

    while (true) {
        // Fetch & decode bb
        auto &cached_bb = m_bb_cache.find(m_hart.pc());
        if (cached_bb.getVirtAddr() != m_hart.pc()) {
            auto fetch = Fetch(m_hart.pc(), *this);
            cached_bb.update(m_hart.pc(), fetch);
        }

        // Execute
        const auto *instrs = cached_bb.instrs();
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
