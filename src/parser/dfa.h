#pragma once

#ifndef EPR_PARSER_DFA_H
#  define EPR_PARSER_DFA_H

#  include "item_set.h"

#  include <string>
#  include <vector>

namespace epr {

struct Dfa {
  using State = ItemSet;

  std::vector<State> states{};
  std::vector<std::map<Symbol, usize>> transitions{};

  explicit Dfa(Grammar &grammar);

  [[nodiscard]] std::string sets_to_string() const;

  [[nodiscard]] std::string transitions_to_string() const;
};

} // namespace epr

#endif // !EPR_PARSER_DFA_H
