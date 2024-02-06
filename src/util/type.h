#pragma once

#ifndef EPR_UTIL_TYPE_H
#  define EPR_UTIL_TYPE_H

#  include <cstdint>

namespace epr {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::size_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using isize = std::ptrdiff_t;

struct Void {};

} // namespace epr

#endif // !EPR_UTIL_TYPE_H
