#pragma once

#ifndef EPR_PARSER_ITEM_SET_H
#  define EPR_PARSER_ITEM_SET_H

#  include "parser/grammar.h"
#  include "parser/item.h"

#  include <set>
#  include <string>

namespace epr {

struct ItemSet {
  std::set<Item> items{};

  ItemSet() = default;

  bool operator==(const ItemSet &rhs) const;

  void push(const Item &item);

  [[nodiscard]] std::set<Symbol> next_symbols() const;

  void self_closure(const Grammar &grammar);

  [[nodiscard]] ItemSet go(const Grammar &grammar, const Symbol &symbol) const;

  [[nodiscard]] std::string to_string() const;
};

} // namespace epr

#endif // !EPR_PARSER_ITEM_SET_H
