#pragma once

#ifndef EPR_PARSER_GRAMMAR_H
#  define EPR_PARSER_GRAMMAR_H

#  include "parser/symbol.h"
#  include "util/all.h"

#  include <map>
#  include <set>
#  include <string>
#  include <utility>
#  include <vector>

namespace epr {

using FirstSet = std::map<Symbol, std::set<Symbol>>;

[[nodiscard]] std::string
to_string(const FirstSet &set, const std::string &name);

struct Grammar {
  inline static Symbol END_SYMBOL{"$", Symbol::Type::Terminator};

  FirstSet first_set{};
  std::map<Symbol, std::set<std::vector<Symbol>>> productions{};
  std::map<std::pair<const Symbol, std::vector<Symbol>>, usize>
      production_index{};
  std::vector<std::pair<Symbol, std::vector<Symbol>>> production_list{};
  Symbol start_symbol;

  explicit Grammar(Symbol start_symbol_);

  Grammar(const Grammar &rhs) = default;

  Grammar(Grammar &&rhs) noexcept = default;

  Grammar &operator=(const Grammar &rhs) = default;

  static Grammar from_str(const std::string &str);

  static Grammar from_str(const std::string_view &str);

  [[nodiscard]] std::string to_string() const;

  [[nodiscard]] std::pair<std::set<Symbol>, bool> get_terminators() const;

  [[nodiscard]] std::set<Symbol> get_nonterminators() const;

  void self_augment();

  void push_productions(
      const Symbol &lhs, const std::set<std::vector<Symbol>> &rhs_set
  );

  void
  push_productions(const Symbol &lhs, std::set<std::vector<Symbol>> &&rhs_set);

  void push_production(const Symbol &lhs, const std::vector<Symbol> &rhs);

  void push_production(const Symbol &lhs, std::vector<Symbol> &&rhs);

  void build_first_set();

  void build_production_index();

  [[nodiscard]] std::set<Symbol> first(const std::vector<Symbol> &str) const;
};

} // namespace epr

#endif // !EPR_PARSER_GRAMMAR_H
