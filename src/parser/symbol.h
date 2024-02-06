#pragma once

#ifndef EPR_PARSER_SYMBOL_H
#  define EPR_PARSER_SYMBOL_H

#  include "util/all.h"

#  include <string>
#  include <vector>

namespace epr {

struct Symbol {
  std::string name{};

  enum Type { Terminator, NonTerminator } type;

  Symbol(const Symbol &rhs) = default;

  Symbol(Symbol &&rhs) noexcept = default;

  Symbol &operator=(const Symbol &rhs) = default;

  Symbol &operator=(Symbol &&rhs) noexcept = default;

  Symbol(std::string v, Type type);

  bool operator<(const Symbol &rhs) const;

  bool operator==(const Symbol &rhs) const;

  static Symbol empty_symbol();

  [[nodiscard]] std::string to_string() const;
};

[[nodiscard]] std::string
to_string(const std::pair<const Symbol, std::vector<Symbol>> &production);

} // namespace epr

#endif // !EPR_PARSER_SYMBOL_H
