#pragma once

#ifndef EPR_PARSER_ITEM_H
#  define EPR_PARSER_ITEM_H

#  include "parser/symbol.h"
#  include "util/all.h"

#  include <string>
#  include <vector>

namespace epr {

struct Item {
  Symbol lookahead;
  std::vector<Symbol> rhs{};
  Symbol lhs;
  usize dot_pos{}; // range: [0, rhs.size()]

  Item(
      Symbol lhs_, std::vector<Symbol> rhs_, usize dot_pos_, Symbol lookahead_
  );

  Item(const Item &rhs) = default;

  Item(Item &&rhs) noexcept = default;

  Item &operator=(const Item &rhs) = default;

  Item &operator=(Item &&rhs) noexcept = default;

  bool operator==(const Item &other) const;

  bool operator<(const Item &other) const;

  [[nodiscard]] Item advance_dot() const;

  [[nodiscard]] bool is_mergeable(const Item &other) const;

  // void merge(const Item &other);

  [[nodiscard]] std::vector<Symbol> back_slice(usize offset = 1) const;

  [[nodiscard]] std::string to_string() const;
};

} // namespace epr

#endif // !EPR_PARSER_ITEM_H
