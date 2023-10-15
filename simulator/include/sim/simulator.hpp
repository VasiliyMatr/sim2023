#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/instr.hpp>
#include <sim/hart.hpp>
#include <sim/memory.hpp>

namespace sim {

struct Simulator final {
    enum class SimStatus {
        OK,
    };

  private:
    memory::PhysMemory m_phys_memory;

    hart::Hart m_hart{m_phys_memory};

    template<instr::InstrId>
    SimStatus simInstr(const instr::Instr &instr) noexcept;

  public:
    template <class It> void binLoad(PhysAddr base_addr, It begin, It end);
    SimStatus simulate(PhysAddr start_pc);
};

} // namespace sim

#endif // SIMULATOR_HPP
