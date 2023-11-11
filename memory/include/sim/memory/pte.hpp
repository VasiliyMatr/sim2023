#ifndef INCL_PTE_HPP
#define INCL_PTE_HPP

#include <sim/common.hpp>

namespace sim::memory {

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
    constexpr PTEFlags(uint8_t flags = 0) : m_flags(flags & ALL_MASK) {}

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

static constexpr bit::BitIdx PTE_PPN_HI = 53;
static constexpr bit::BitIdx PTE_PPN_LO = 10;

NODISCARD inline PTE createPTE(PPN ppn, PTEFlags flags) noexcept {
    return bit::setBitField<PTE>(PTE_PPN_HI, PTE_PPN_LO, flags.raw(), ppn);
}

} // namespace sim::memory

#endif // INCL_PTE_HPP
