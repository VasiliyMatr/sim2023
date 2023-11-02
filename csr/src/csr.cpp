#include <sim/csr.hpp>

namespace sim::csr {

template <> NODISCARD const SATP64 &CSRFile::get() const noexcept {
    return m_satp64;
}

template <> void CSRFile::set(const SATP64 &value) noexcept {
    m_satp64 = value;
}

template<>
NODISCARD CSRFile::AccessStatus CSRFile::write<XLen::XLEN_64>(CSRIdx idx, RawCSRValue<XLen::XLEN_64> value) noexcept {
    switch (idx) {
    case CSRIdx::SATP:
        set<XLen::XLEN_64, CSRIdx::SATP>(value);
        break;

    default:
        return AccessStatus::CSR_NOT_SUPPORTED;
    }

    return AccessStatus::OK;
}

} // namespace sim::csr

