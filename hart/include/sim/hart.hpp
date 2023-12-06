#ifndef INCL_HART_HPP
#define INCL_HART_HPP

#include <cstdint>

#include <sim/common.hpp>
#include <sim/csr.hpp>
#include <sim/gpr.hpp>
#include <sim/memory.hpp>

namespace sim {
namespace hart {

class Hart final {
    VirtAddr m_pc = 0;
    gpr::GPRFile m_gpr_file{};
    csr::CSRFile m_csr_file{};

    memory::PhysMemory &m_phys_memory;
    memory::MMU64 m_mmu64{m_phys_memory,
                          m_csr_file.get<XLen::XLEN_64, csr::CSRIdx::MSTATUS>(),
                          m_csr_file.get<XLen::XLEN_64, csr::CSRIdx::SATP>()};

  public:
    Hart(memory::PhysMemory &phys_memory) : m_phys_memory(phys_memory) {}

    NODISCARD auto pc() const noexcept { return m_pc; }
    NODISCARD auto &pc() noexcept { return m_pc; }

    NODISCARD const auto &gprFile() const noexcept { return m_gpr_file; }
    NODISCARD auto &gprFile() noexcept { return m_gpr_file; }

    NODISCARD const auto &csrFile() const noexcept { return m_csr_file; }
    NODISCARD auto &csrFile() noexcept { return m_csr_file; }

    NODISCARD const auto &physMemory() const noexcept { return m_phys_memory; }
    NODISCARD auto &physMemory() noexcept { return m_phys_memory; }

    NODISCARD const auto &mmu64() const noexcept { return m_mmu64; }
    NODISCARD auto &mmu64() noexcept { return m_mmu64; }
};

} // namespace hart
} // namespace sim

#endif // INCL_HART_HPP
