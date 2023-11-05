#ifndef INCL_MEMORY_HPP
#define INCL_MEMORY_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <sim/common.hpp>
#include <sim/csr/idx.gen.hpp>
#include <sim/csr/value.gen.hpp>

namespace sim {
namespace memory {

struct PhysMemory final {
    enum class AccessStatus { OK, RANGE_ERROR };

  private:
    std::vector<uint8_t> m_data{};
    PhysAddr m_base_addr = 0;

    NODISCARD auto getAccessStatus(PhysAddr phys_addr,
                                   size_t access_size) const noexcept {
        SIM_ASSERT(access_size > 0);

        if (phys_addr < m_base_addr) {
            return AccessStatus::RANGE_ERROR;
        }

        auto offset = phys_addr - m_base_addr;
        if (offset + access_size > m_data.size()) {
            return AccessStatus::RANGE_ERROR;
        }

        return AccessStatus::OK;
    }

  public:
    PhysMemory(PhysAddr base_addr, size_t size)
        : m_base_addr(base_addr), m_data(size) {}

    NODISCARD auto baseAddr() const noexcept { return m_base_addr; }
    NODISCARD auto size() const noexcept { return m_data.size(); }

    template <class UInt>
    NODISCARD AccessStatus read(PhysAddr phys_addr, UInt &dst) const noexcept {
        static_assert(std::is_integral_v<UInt>);

        if (auto status = getAccessStatus(phys_addr, sizeof(UInt));
            status != AccessStatus::OK) {
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        dst = *reinterpret_cast<const UInt *>(m_data.data() + offset);

        return AccessStatus::OK;
    }

    template <class UInt>
    NODISCARD AccessStatus write(PhysAddr phys_addr, UInt value) {
        static_assert(std::is_integral_v<UInt>);

        if (auto status = getAccessStatus(phys_addr, sizeof(UInt));
            status != AccessStatus::OK) {
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        *reinterpret_cast<UInt *>(m_data.data() + offset) = value;

        return AccessStatus::OK;
    }

    NODISCARD AccessStatus
    getConstHostPtr(PhysAddr phys_addr, size_t access_size,
                    const uint8_t *&host_ptr) const noexcept {
        SIM_ASSERT(access_size > 0);

        if (auto status = getAccessStatus(phys_addr, access_size);
            status != AccessStatus::OK) {
            host_ptr = nullptr;
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        host_ptr = m_data.data() + offset;
        return AccessStatus::OK;
    }

    NODISCARD AccessStatus getMuteHostPtr(PhysAddr phys_addr,
                                          size_t access_size,
                                          uint8_t *&host_ptr) noexcept {
        SIM_ASSERT(access_size > 0);

        if (auto status = getAccessStatus(phys_addr, access_size);
            status != AccessStatus::OK) {
            host_ptr = nullptr;
            return status;
        }

        auto offset = phys_addr - m_base_addr;
        host_ptr = m_data.data() + offset;
        return AccessStatus::OK;
    }
};

static constexpr bit::BitSize PAGE_BIT_SIZE = 12;
static constexpr size_t PAGE_SIZE = 1 << PAGE_BIT_SIZE;

// Physical page number
using PPN = uint64_t;

// Virtual page number
using VPN = uint64_t;

// Page Table Entry
using PTE = uint64_t;

// PTE reserved bits mask.
// Svnapot and Svpbmt extensions are not supported
static constexpr PTE PTE_RESERWED_MASK = (PTE{0x3FF} << 54) | (PTE{3} << 8);

struct PTEFlags final {
    enum FlagsMask : uint8_t {
        V_MASK = 1 << 0,
        R_MASK = 1 << 1,
        W_MASK = 1 << 2,
        X_MASK = 1 << 3,
        U_MASK = 1 << 4,
        G_MASK = 1 << 5,
        A_MASK = 1 << 6,
        D_MASK = 1 << 7,

        ALL_MASK = 0x7F,
    };

  private:
    uint8_t m_flags = 0;

  public:
    PTEFlags(uint8_t flags = 0) : m_flags(flags & ALL_MASK) {}

    NODISCARD auto raw() const noexcept { return m_flags; }

    NODISCARD bool v() const noexcept { return m_flags & V_MASK; }
    NODISCARD bool r() const noexcept { return m_flags & R_MASK; }
    NODISCARD bool w() const noexcept { return m_flags & W_MASK; }
    NODISCARD bool x() const noexcept { return m_flags & X_MASK; }
    NODISCARD bool u() const noexcept { return m_flags & U_MASK; }
    NODISCARD bool g() const noexcept { return m_flags & G_MASK; }
    NODISCARD bool a() const noexcept { return m_flags & A_MASK; }
    NODISCARD bool d() const noexcept { return m_flags & D_MASK; }
};

// V, A and D are set by default
NODISCARD inline PTE createValidPTE(PPN ppn, PTEFlags flags) noexcept {
    static constexpr bit::BitIdx PPN_HI = 53;
    static constexpr bit::BitIdx PPN_LO = 10;

    static constexpr uint8_t DEFAULT_FLAGS = PTEFlags::V_MASK & PTEFlags::A_MASK & PTEFlags::D_MASK;

    return bit::setBitField<PTE>(PPN_HI, PPN_LO, flags.raw() & DEFAULT_FLAGS, ppn);
}

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

} // namespace memory
} // namespace sim

#endif // INCL_MEMORY_HPP
