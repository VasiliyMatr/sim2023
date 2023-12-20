#ifndef INCL_MEMORY_PTE_HPP
#define INCL_MEMORY_PTE_HPP

#include <sim/memory/common.hpp>

namespace sim::memory {

// Page Table Entry
using PTE = uint64_t;

// PTE reserved bits mask.
// Svnapot and Svpbmt extensions are not supported
static constexpr PTE PTE_RESERWED_MASK = (PTE{0x3FF} << 54) | (PTE{3} << 8);

// Page table entry flags
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
    };

  private:
    uint8_t m_flags = 0;

  public:
    constexpr PTEFlags(uint8_t flags = 0) : m_flags(flags) {}

    NODISCARD constexpr auto raw() const noexcept { return m_flags; }

    NODISCARD constexpr bool v() const noexcept { return m_flags & V_MASK; }
    NODISCARD constexpr bool r() const noexcept { return m_flags & R_MASK; }
    NODISCARD constexpr bool w() const noexcept { return m_flags & W_MASK; }
    NODISCARD constexpr bool x() const noexcept { return m_flags & X_MASK; }
    NODISCARD constexpr bool u() const noexcept { return m_flags & U_MASK; }
    NODISCARD constexpr bool g() const noexcept { return m_flags & G_MASK; }
    NODISCARD constexpr bool a() const noexcept { return m_flags & A_MASK; }
    NODISCARD constexpr bool d() const noexcept { return m_flags & D_MASK; }
};

// PTE PPN field hi bit index
static constexpr bit::BitIdx PTE_PPN_HI = 53;
// PTE PPN field lo bit index
static constexpr bit::BitIdx PTE_PPN_LO = 10;

// Create PTE for given PPN and flags
NODISCARD constexpr inline PTE createPTE(PPN ppn, PTEFlags flags) noexcept {
    auto pte = bit::setBitField<PTE>(PTE_PPN_HI, PTE_PPN_LO, flags.raw(), ppn);
    SIM_ASSERT(!(pte & PTE_RESERWED_MASK));

    return pte;
}

} // namespace sim::memory

#endif // INCL_MEMORY_PTE_HPP
