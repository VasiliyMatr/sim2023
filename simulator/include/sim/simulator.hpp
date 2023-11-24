#ifndef INCL_SIM_SIMULATOR_HPP
#define INCL_SIM_SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>

namespace sim {

class Simulator final {
    memory::PhysMemory m_phys_memory{};

    hart::Hart m_hart{m_phys_memory};

    template <instr::InstrId>
    SimStatus simInstr(const instr::Instr &instr) noexcept;

  public:
    hart::Hart &getHart();
    memory::PhysMemory &getPhysMemory();

    SimStatus simulate(PhysAddr start_pc);
};

} // namespace sim

#endif // INCL_SIM_SIMULATOR_HPP
