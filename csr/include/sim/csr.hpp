#ifndef INCL_CSR_HPP
#define INCL_CSR_HPP

#include <array>

#include <sim/common.hpp>

#include <sim/csr/idx.gen.hpp>
#include <sim/csr/value.gen.hpp>

namespace sim::csr {

NODISCARD constexpr inline bool isCSRWritable(CSRIdx idx) noexcept {
    constexpr bit::BitIdx HI = 11;
    constexpr bit::BitIdx LO = 10;

    constexpr uint16_t READ_ONLY_CODE = 0b11;

    return bit::getBitField<uint16_t, HI, LO>(to_underlying(idx)) ==
           READ_ONLY_CODE;
}

NODISCARD constexpr inline PrivLevel getCSRPrivLevel(CSRIdx idx) noexcept {
    constexpr bit::BitIdx HI = 9;
    constexpr bit::BitIdx LO = 8;

    constexpr uint16_t HYPERVISOR_CODE = 0b10;

    auto raw_priv_level =
        bit::getBitField<uint16_t, HI, LO>(to_underlying(idx));

    if (raw_priv_level == HYPERVISOR_CODE) {
        return PrivLevel::MACHINE;
    }

    return PrivLevel{raw_priv_level};
}

struct CSRFile final {
    enum class AccessStatus { OK, CSR_NOT_SUPPORTED };

  private:
    CSRValue<XLen::XLEN_64, CSRIdx::SATP> m_satp64{};

  public:
    template <XLen xlen>
    NODISCARD AccessStatus read(CSRIdx idx,
                                RawCSRValue<xlen> &dst) const noexcept {
        switch (idx) {
        case CSRIdx::SATP:
            dst = get<xlen, CSRIdx::SATP>().getValue();
            break;

        default:
            return AccessStatus::CSR_NOT_SUPPORTED;
        }

        return AccessStatus::OK;
    }

    template <XLen xlen>
    NODISCARD AccessStatus write(CSRIdx idx, RawCSRValue<xlen> value) noexcept;

    template <XLen xlen, CSRIdx idx>
    NODISCARD const CSRValue<xlen, idx> &get() const noexcept;

    template <XLen xlen, CSRIdx idx>
    void set(const CSRValue<xlen, idx> &value) noexcept;
};

} // namespace sim::csr

#endif // INCL_CSR_HPP
