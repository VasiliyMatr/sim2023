#ifndef INCL_SIMULATOR_HPP
#define INCL_SIMULATOR_HPP

#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/memory.hpp>
#include <sim/instr.hpp>
namespace sim {

constexpr sim::PhysAddr DATA_SEGMENT_BASE_ADDR = 0x5000000000;
constexpr size_t SIZE_16MB = size_t{1} << 24;
struct Simulator final {
  public:
    enum class SimStatus {
        OK,
        NOT_IMPLEMENTED_INSTR,
        PHYS_MEMORY_ERROR,
        PC_ALIGN_ERROR,
    };

  private:
    memory::PhysMemory m_phys_memory{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};

    hart::Hart m_hart{m_phys_memory};

    template<instr::InstrId> SimStatus simInstr(const instr::Instr &instr) noexcept;

  public:
    Simulator(memory::PhysMemory &memory, hart::Hart &hart) : m_phys_memory(memory), m_hart(hart) {};

    Simulator() = default;
  
    hart::Hart &getHart();
  
    void addInstructionsToMemory(const std::vector<InstrCode>& instructions, size_t memorySize);
  
    SimStatus simulate(PhysAddr start_pc);
};

} // namespace sim

#endif // INCL_SIMULATOR_HPP
