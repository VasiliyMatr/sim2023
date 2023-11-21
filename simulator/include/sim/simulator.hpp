#ifndef INCL_SIMULATOR_HPP
#define INCL_SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/memory.hpp>
#include <sim/instr.hpp>

namespace sim {

struct Simulator final {
  public:
    enum class SimStatus {
        OK,
        NOT_IMPLEMENTED_INSTR,
        PHYS_MEMORY_ERROR,
        PC_ALIGN_ERROR,
    };

  private:
    memory::PhysMemory &m_phys_memory;

    hart::Hart m_hart{m_phys_memory};

    template<instr::InstrId> SimStatus simInstr(const instr::Instr &instr) noexcept;

  public:
    Simulator(hart::Hart &hart, memory::PhysMemory &memory) : m_hart(hart), m_phys_memory{memory} {};
    ~Simulator() = default;
    void addInstructionsToMemory(const std::vector<InstrCode>& instructions, memory::PhysMemory &memory, size_t memorySize);
    SimStatus simulate(PhysAddr start_pc);
};

} // namespace sim

#endif // INCL_SIMULATOR_HPP
