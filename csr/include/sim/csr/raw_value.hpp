#ifndef INCL_RAW_VALUE_HPP
#define INCL_RAW_VALUE_HPP

#include <sim/common.hpp>

namespace sim::csr {

template <XLen xlen> struct RawCSRValueType;

template <> struct RawCSRValueType<XLen::XLEN_32> { using Type = uint32_t; };
template <> struct RawCSRValueType<XLen::XLEN_64> { using Type = uint64_t; };

template <XLen xlen> using RawCSRValue = typename RawCSRValueType<xlen>::Type;

using RawCSRValue32 = RawCSRValue<XLen::XLEN_32>;
using RawCSRValue64 = RawCSRValue<XLen::XLEN_64>;

} // namespace sim::csr

#endif // INCL_RAW_VALUE_HPP
