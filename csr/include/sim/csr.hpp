#ifndef INCL_CSR_HPP
#define INCL_CSR_HPP

#include <array>

#include <sim/common.hpp>

namespace sim {
namespace csr {

enum class CSRIdx {
    SSTATUS = 0x100,
    SATP = 0x180,
};

NODISCARD inline bool isSupported(CSRIdx idx) noexcept;
NODISCARD inline bool isReadable(CSRIdx idx, PrivLevel priv_level) noexcept;
NODISCARD inline bool isWritable(CSRIdx idx, PrivLevel priv_level) noexcept;

template <XLen xlen> struct RawCSRValueType;

template <> struct RawCSRValueType<XLen::XLEN_32> { using Type = uint32_t; };
template <> struct RawCSRValueType<XLen::XLEN_64> { using Type = uint64_t; };

template <XLen xlen> using RawCSRValue = typename RawCSRValueType<xlen>::Type;

template <XLen xlen> struct BaseCSRValue {
    using RawValue = RawCSRValue<xlen>;

  private:
    RawValue m_value = 0;

  protected:
    BaseCSRValue(RawValue value) : m_value(value) {}

  public:
    NODISCARD auto getValue() const noexcept { return m_value; }
    void setValue(RawValue value) noexcept { m_value = value; }
};

using BaseCSRValue32 = BaseCSRValue<XLen::XLEN_32>;
using BaseCSRValue64 = BaseCSRValue<XLen::XLEN_64>;

template <XLen xlen, CSRIdx idx> class CSRValue;

struct CSRFile final {
    enum class AccessStatus { OK, UNKNOWN_CSR };

  private:
    static constexpr size_t CSR_NUMBER = 4096;

    std::array<RawCSRValue<XLen::XLEN_64>, CSR_NUMBER> m_CSRs{};

  public:
    template <XLen xlen>
    NODISCARD AccessStatus read(CSRIdx idx,
                                RawCSRValue<xlen> &dst) const noexcept {
        if (isSupported(idx)) {
            dst = m_CSRs[to_underlying(idx)];
            return AccessStatus::OK;
        }

        return AccessStatus::UNKNOWN_CSR;
    }

    template <XLen xlen>
    NODISCARD AccessStatus write(CSRIdx idx, RawCSRValue<xlen> value) noexcept {
        if (isSupported(idx)) {
            m_CSRs[to_underlying(idx)] = value;
            return AccessStatus::OK;
        }

        return AccessStatus::UNKNOWN_CSR;
    }

    template <XLen xlen, CSRIdx idx>
    NODISCARD CSRValue<xlen, idx> read() const noexcept {
        return m_CSRs[to_underlying(idx)];
    }

    template <XLen xlen, CSRIdx idx>
    void write(CSRValue<xlen, idx> value) noexcept {
        m_CSRs[to_underlying(idx)] = value.getValue();
    }
};

} // namespace csr
} // namespace sim

#endif // INCL_CSR_HPP
