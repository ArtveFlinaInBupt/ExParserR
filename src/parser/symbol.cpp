#include "parser/symbol.h"

#include "util/all.h"

#include <format>
#include <vector>

namespace epr {

Symbol::Symbol(std::string v, const Type type):
    name(std::move(v)), type(type) {}

bool Symbol::operator<(const Symbol &rhs) const {
  return name < rhs.name;
}

bool Symbol::operator==(const Symbol &rhs) const {
  return name == rhs.name;
}

Symbol Symbol::empty_symbol() {
  return {"", Symbol::Terminator};
}

std::string Symbol::to_string() const {
  if (name.empty())
    return "~";
  return name;
}

[[nodiscard]] std::string
to_string(const std::pair<const Symbol, std::vector<Symbol>> &production) {
  const auto &[lhs, rhs] = production;
  std::string buf = std::format("{} -> ", lhs.to_string());
  for (const auto &symbol : rhs)
    buf.append(symbol.to_string()).append(1, ' ');
  buf.pop_back();
  return buf;
}

} // namespace epr
