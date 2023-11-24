#ifndef INCL_SIM_CSR_HPP
#define INCL_SIM_CSR_HPP

#include <array>

#include <sim/common.hpp>

#include <sim/csr/idx.gen.hpp>
#include <sim/csr/value.gen.hpp>

namespace sim::csr {

NODISCARD constexpr inline bool isCSRWritable(CSRIdx idx) noexcept {
    constexpr bit::BitIdx hi = 11;
    constexpr bit::BitIdx lo = 10;

    constexpr uint16_t READ_ONLY_CODE = 0b11;

    return bit::getBitField<uint16_t>(hi, lo, to_underlying(idx)) ==
           READ_ONLY_CODE;
}

NODISCARD constexpr inline PrivLevel getCSRPrivLevel(CSRIdx idx) noexcept {
    constexpr bit::BitIdx hi = 9;
    constexpr bit::BitIdx lo = 8;

    constexpr uint16_t HYPERVISOR_CODE = 0b10;

    auto raw_priv_level =
        bit::getBitField<uint16_t>(hi, lo, to_underlying(idx));

    if (raw_priv_level == HYPERVISOR_CODE) {
        return PrivLevel::MACHINE;
    }

    return PrivLevel{raw_priv_level};
}

class CSRFile final {
    CSRValue<XLen::XLEN_64, CSRIdx::SATP> m_satp64{};

  public:
    template <XLen xlen>
    NODISCARD SimStatus read(CSRIdx idx, RawCSRValue<xlen> &dst) const noexcept;

    template <XLen xlen>
    NODISCARD SimStatus write(CSRIdx idx, RawCSRValue<xlen> value) noexcept;

    template <XLen xlen, CSRIdx idx>
    NODISCARD const CSRValue<xlen, idx> &get() const noexcept;

    template <XLen xlen, CSRIdx idx>
    void set(const CSRValue<xlen, idx> &value) noexcept;
};

} // namespace sim::csr

#endif // INCL_SIM_CSR_HPP
