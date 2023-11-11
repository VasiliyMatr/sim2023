#ifndef INCL_MMU64_HPP
#define INCL_MMU64_HPP

#include <sim/csr/idx.gen.hpp>
#include <sim/csr/value.gen.hpp>

#include <sim/memory/phys_memory.hpp>
#include <sim/memory/pte.hpp>

namespace sim::memory {

// Translates VirtAddr -> PhysAddr in 64-bit mode
struct MMU64 final {
    enum class AccessType { READ, WRITE, FETCH };
    enum class Status { ACCESS_FAULT, PAGE_FAULT, OK, UNDEF };

    class Result final {
        Status m_status = Status::UNDEF;
        PhysAddr m_phys_addr = 0;

      public:
        Result(Status status, PhysAddr phys_addr)
            : m_status(status), m_phys_addr(phys_addr) {}

        NODISCARD auto status() const noexcept { return m_status; }
        NODISCARD auto physAddr() const noexcept { return m_phys_addr; }
    };

  private:
    PhysMemory &m_phys_memory;
    const csr::MSTATUS64 &m_mstatus64;
    const csr::SATP64 &m_satp64;

  public:
    MMU64(PhysMemory &phys_memory, const csr::MSTATUS64 &mstatus64,
          const csr::SATP64 &satp64)
        : m_phys_memory(phys_memory), m_mstatus64(mstatus64), m_satp64(satp64) {
    }

    NODISCARD Result translate(PrivLevel priv_level, AccessType access_type,
                               VirtAddr va) noexcept;
};

} // namespace sim::memory

#endif // INCL_MMU64_HPP
