#ifndef INCL_SIMULATOR_HPP
#define INCL_SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/memory.hpp>

namespace sim {

struct Simulator final {
    enum class SimStatus {
        OK,
        ERROR,
        NOT_IMPLEMENTED_INSTR,
        PHYS_MEMORY_ERROR,
        PC_ALIGN_ERROR,
    };

  private:
    memory::PhysMemory m_phys_memory;

    hart::Hart m_hart{m_phys_memory};

  public:
    template<instr::InstrId> SimStatus simInstr(const instr::Instr &instr) noexcept;
    
    template <class It> void binLoad(PhysAddr base_addr, It begin, It end);
    SimStatus simulate(PhysAddr start_pc);
};

} // namespace sim

#endif // INCL_SIMULATOR_HPP
