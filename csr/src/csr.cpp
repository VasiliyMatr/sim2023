#include <sim/csr.hpp>

namespace sim::csr {

template <> NODISCARD const MSTATUS64 &CSRFile::get() const noexcept {
    return m_mstatus64;
}

template <> void CSRFile::set(const MSTATUS64 &value) noexcept {
    m_mstatus64 = value;
}

template <> NODISCARD const SATP64 &CSRFile::get() const noexcept {
    return m_satp64;
}

template <> void CSRFile::set(const SATP64 &value) noexcept {
    m_satp64 = value;
}

template <>
NODISCARD SimStatus
CSRFile::read<XLen::XLEN_64>(CSRIdx idx, RawCSRValue64 &dst) const noexcept {
    switch (idx) {
    case CSRIdx::SATP:
        dst = get<XLen::XLEN_64, CSRIdx::SATP>().getValue();
        break;

    case CSRIdx::MSTATUS:
        dst = get<XLen::XLEN_64, CSRIdx::MSTATUS>().getValue();
        break;

    default:
        return SimStatus::CSR__NOT_SUPPORTED;
    }

    return SimStatus::OK;
}

template <>
NODISCARD SimStatus
CSRFile::write<XLen::XLEN_64>(CSRIdx idx, RawCSRValue64 value) noexcept {
    switch (idx) {
    case CSRIdx::SATP:
        set<XLen::XLEN_64, CSRIdx::SATP>(value);
        break;

    case CSRIdx::MSTATUS:
        set<XLen::XLEN_64, CSRIdx::MSTATUS>(value);
        break;

    default:
        return SimStatus::CSR__NOT_SUPPORTED;
    }

    return SimStatus::OK;
}

} // namespace sim::csr
