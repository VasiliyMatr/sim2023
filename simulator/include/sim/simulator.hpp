#ifndef INCL_SIM_SIMULATOR_HPP
#define INCL_SIM_SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>

namespace sim {

constexpr sim::PhysAddr PHYS_MEM_BASE_ADDR = 0x5000000000;
constexpr size_t SIZE_16MB = size_t{1} << 24;
struct Simulator final {
    enum class SimStatus {
        OK,
        NOT_IMPLEMENTED_INSTR,
        PHYS_MEMORY_ERROR,
        PC_ALIGN_ERROR,
        EXIT,
    };

  private:
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
