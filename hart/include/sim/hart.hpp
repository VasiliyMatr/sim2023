#ifndef INCL_HART_HPP
#define INCL_HART_HPP

#include <cstdint>

#include <sim/common.hpp>
#include <sim/gpr.hpp>
#include <sim/memory.hpp>

namespace sim {
namespace hart {

class Hart final {
    PhysAddr m_pc = 0;
    gpr::GPRFile m_gpr_file{};

    memory::PhysMemory &m_phys_memory;

  public:
    Hart(memory::PhysMemory &phys_memory) : m_phys_memory(phys_memory) {}

    NODISCARD auto pc() const noexcept { return m_pc; }
    NODISCARD auto &pc() noexcept { return m_pc; }
    NODISCARD const auto &gprFile() const noexcept { return m_gpr_file; }
    NODISCARD auto &gprFile() noexcept { return m_gpr_file; }
    NODISCARD const auto &physMemory() const noexcept { return m_phys_memory; }
    NODISCARD auto &physMemory() noexcept { return m_phys_memory; }
};

} // namespace hart
} // namespace sim

#endif // INCL_HART_HPP
