#pragma once

#ifndef EPR_UTIL_FUNCTIONAL_H
#  define EPR_UTIL_FUNCTIONAL_H

#  include "util/type.h"

#  include <string>
#  include <vector>

namespace epr {

constexpr std::vector<std::string>
split(const std::string &str, const char sep) {
  std::vector<std::string> result{};
  std::string::size_type pos = 0;
  while (pos < str.size()) {
    const auto next_pos = str.find(sep, pos);
    if (next_pos == std::string::npos) {
      result.push_back(str.substr(pos));
      break;
    }
    result.push_back(str.substr(pos, next_pos - pos));
    pos = next_pos + 1;
  }
  return result;
}

constexpr std::vector<std::string>
split(const std::string &str, const std::string &sep) {
  std::vector<std::string> result{};
  std::string::size_type pos = 0;
  while (pos < str.size()) {
    const auto next_pos = str.find(sep, pos);
    if (next_pos == std::string::npos) {
      result.push_back(str.substr(pos));
      break;
    }
    result.push_back(str.substr(pos, next_pos - pos));
    pos = next_pos + sep.size();
  }
  return result;
}

inline usize len(usize n) {
  usize r = 0;
  do
    ++r;
  while (n /= 10);
  return r;
}

} // namespace epr

#endif // !EPR_UTIL_FUNCTIONAL_H
