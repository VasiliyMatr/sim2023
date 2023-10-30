#ifndef INCL_CSR_HPP
#define INCL_CSR_HPP

#include <array>

#include <sim/common.hpp>

#include <sim/csr/idx.gen.hpp>
#include <sim/csr/value.gen.hpp>

namespace sim::csr {

NODISCARD constexpr inline bool isCSRSupported(CSRIdx idx) noexcept {
    switch (idx) {
    case CSRIdx::SATP:
        return true;
    default:
        return false;
    }

    SIM_ASSERT(false);
}

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
    static constexpr size_t CSR_NUMBER = 4096;

    std::array<RawCSRValue<XLen::XLEN_64>, CSR_NUMBER> m_CSRs{};

  public:
    template <XLen xlen>
    NODISCARD constexpr AccessStatus
    read(CSRIdx idx, RawCSRValue<xlen> &dst) const noexcept {
        if (isCSRSupported(idx)) {
            dst = m_CSRs[to_underlying(idx)];
            return AccessStatus::OK;
        }

        return AccessStatus::CSR_NOT_SUPPORTED;
    }

    template <XLen xlen>
    NODISCARD constexpr AccessStatus write(CSRIdx idx,
                                           RawCSRValue<xlen> value) noexcept {
        if (isCSRSupported(idx)) {
            m_CSRs[to_underlying(idx)] = value;
            return AccessStatus::OK;
        }

        return AccessStatus::CSR_NOT_SUPPORTED;
    }

    template <XLen xlen, CSRIdx idx>
    NODISCARD CSRValue<xlen, idx> read() const noexcept {
        static_assert(isCSRSupported(idx));

        return m_CSRs[to_underlying(idx)];
    }

    template <XLen xlen, CSRIdx idx>
    void write(CSRValue<xlen, idx> value) noexcept {
        static_assert(isCSRSupported(idx));

        m_CSRs[to_underlying(idx)] = value.getValue();
    }
};

} // namespace sim::csr

#endif // INCL_CSR_HPP
