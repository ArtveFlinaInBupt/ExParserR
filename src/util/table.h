#pragma once

#ifndef EPR_UTIL_TABLE_H
#  define EPR_UTIL_TABLE_H

#  include "type.h"

#  include <cassert>
#  include <format>
#  include <functional>
// #  include <numeric>
#  include <string>
#  include <string_view>
#  include <utility>
#  include <vector>

namespace epr {

using Table = std::vector<std::vector<std::string>>;

enum class Align : u8 {
  Left,
  Center,
  Right,
};

constexpr std::string vline(
    const std::vector<usize> &widths, const std::vector<std::string_view> &sep
) {
  assert(sep.size() == 3); // left, middle, right
  std::string buf{};
  buf.append(sep[0]);
  for (usize i = 0; i < widths.size(); ++i) {
    for (usize _ = 0; _ < widths[i] + 2; ++_)
      buf.append("─");
    if (i != widths.size() - 1)
      buf.append(sep[1]);
  }
  buf.append(sep[2]);
  return buf;
}

constexpr std::string top_rule(const std::vector<usize> &widths) {
  return vline(widths, {"┌", "┬", "┐"});
}

constexpr std::string mid_rule(const std::vector<usize> &widths) {
  return vline(widths, {"├", "┼", "┤"});
}

constexpr std::string bottom_rule(const std::vector<usize> &widths) {
  return vline(widths, {"└", "┴", "┘"});
}

constexpr std::string
cell(const std::string &s, usize width, const Align align) {
  switch (align) {
    case Align::Left:
      return std::format(" {:<{}} ", s, width);
    case Align::Center:
      return std::format(" {:^{}} ", s, width);
    case Align::Right:
      return std::format(" {:>{}} ", s, width);
    default:
      std::unreachable();
  }
}

// clang-format off
template<typename F>
  requires std::is_invocable_r_v<Align, F, usize, usize>
constexpr std::string to_string(const Table &table, F &&align) {
  // clang-format on
  assert(!table.empty() && !table[0].empty());
  for (const auto &row : table)
    assert(row.size() == table[0].size());

  std::vector<usize> max_widths(table[0].size());

  for (const auto &row : table)
    for (usize i = 0; i < row.size(); ++i)
      max_widths[i] = std::max(max_widths[i], row[i].size());

  std::string buf{};

  // { // reserve buffer
  //   const usize row_width =
  //       std::accumulate(max_widths.begin(), max_widths.end(), usize{0}) +
  //       6 * max_widths.size() + 4;
  //   const usize rule_width =
  //       std::accumulate(max_widths.begin(), max_widths.end(), usize{0}) * 3 +
  //       9 * max_widths.size() + 4;
  //   buf.reserve(row_width * table.size() + rule_width * 3 + 30);
  // }

  { // header
    buf.append("\033[1;33m").append(top_rule(max_widths)).append("\n");

    const auto &header = table[0];
    buf.append("│");
    for (usize j = 0; j < header.size(); ++j)
      buf.append(cell(header[j], max_widths[j], align(0, j))).append("│");

    buf.append("\n")
        .append(mid_rule(max_widths))
        .append("\033[0m")
        .append("\n");
  }

  // body
  for (usize i = 1; i < table.size(); ++i) {
    const auto &row = table[i];

    buf.append("│");
    for (usize j = 0; j < row.size(); ++j)
      buf.append(cell(row[j], max_widths[j], align(i, j))).append("│");

    buf.append("\n");
  }

  // footer
  buf.append(bottom_rule(max_widths));

  return buf;
}

} // namespace epr

#endif // EPR_UTIL_TABLE_H
