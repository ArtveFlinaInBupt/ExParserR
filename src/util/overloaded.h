#pragma once

#ifndef EPR_UTIL_OVERLOADED_H
#  define EPR_UTIL_OVERLOADED_H

namespace epr {

template<class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace epr

#endif // !EPR_UTIL_OVERLOADED_H
