// Contains hart module declarations

#ifndef INCL_HART_HPP
#define INCL_HART_HPP

#include <sim/csr/csr.hpp>
#include <sim/gpr/gpr.hpp>
#include <sim/memory/memory.hpp>

namespace sim {
namespace hart {

class Hart final {
    csr::CSRFile m_csr_file{};
    gpr::GPRFile m_gpr_file{};
    memory::PhysMemory m_phys_memory{};
};

} // namespace hart
} // namespace sim

#endif // INCL_HART_HPP
