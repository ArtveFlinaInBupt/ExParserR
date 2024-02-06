#include "parser/item.h"

#include <format>
#include <string>
#include <tuple>
#include <vector>

namespace epr {

Item::Item(
    Symbol lhs_, std::vector<Symbol> rhs_, const usize dot_pos_,
    Symbol lookahead_
):
    lookahead(std::move(lookahead_)), rhs(std::move(rhs_)),
    lhs(std::move(lhs_)), dot_pos(dot_pos_) {}

bool Item::operator==(const Item &other) const {
  return std::tie(lhs, rhs, dot_pos, lookahead) ==
         std::tie(other.lhs, other.rhs, other.dot_pos, other.lookahead);
}

bool Item::operator<(const Item &other) const {
  return std::tie(lhs, rhs, dot_pos, lookahead) <
         std::tie(other.lhs, other.rhs, other.dot_pos, other.lookahead);
}

Item Item::advance_dot() const {
  return {lhs, rhs, dot_pos + 1, lookahead};
}

bool Item::is_mergeable(const Item &other) const {
  return std::tie(lhs, rhs, dot_pos) ==
         std::tie(other.lhs, other.rhs, other.dot_pos);
}

// void Item::merge(const Item &other) {
//   lookahead.insert(other.lookahead.begin(), other.lookahead.end());
// }

std::vector<Symbol> Item::back_slice(const usize offset) const {
  std::vector<Symbol> buf = {
      rhs.begin() + static_cast<isize>(dot_pos + offset), rhs.end()
  };
  buf.reserve(buf.size() + 1);
  buf.push_back(lookahead);
  return buf;
}

std::string Item::to_string() const {
  std::string buf = std::format("{} \033[34m->\033[0m ", lhs.to_string());
  for (usize idx = 0; idx < rhs.size(); ++idx) {
    if (idx == dot_pos)
      buf.append("\033[34m·\033[0m ");
    buf.append(rhs.at(idx).to_string()).append(1, ' ');
  }
  if (dot_pos == rhs.size())
    buf.append("\033[34m·\033[0m ");
  buf.pop_back();
  buf.append(std::format("\033[90m,\033[0m\t  {}", lookahead.to_string()));
  return buf;
}

} // namespace epr
