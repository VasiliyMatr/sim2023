#ifndef INCL_SIMULATOR_HPP
#define INCL_SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/memory.hpp>
#include <sim/instr.hpp>

namespace sim {

class Simulator final {
  public:
    enum class SimStatus {
        OK,
        ERROR,
        NOT_IMPLEMENTED_INSTR,
        PHYS_MEMORY_ERROR,
        PC_ALIGN_ERROR,
    };

  private:
    memory::PhysMemory &m_phys_memory;

    hart::Hart m_hart{m_phys_memory};

  public:
    Simulator(hart::Hart &hart, memory::PhysMemory &memory) : m_hart(hart), m_phys_memory{memory} {};
    ~Simulator() = default;
    template<instr::InstrId> SimStatus simInstr(const instr::Instr &instr) noexcept;
    SimStatus simulate(PhysAddr start_pc);
};

} // namespace sim

#endif // INCL_SIMULATOR_HPP
