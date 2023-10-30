#ifndef INCL_RAW_VALUE_HPP
#define INCL_RAW_VALUE_HPP

#include <sim/common.hpp>

namespace sim::csr {

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

} // namespace sim::csr

#endif // INCL_RAW_VALUE_HPP
