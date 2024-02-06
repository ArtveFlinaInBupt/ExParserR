#include "parser/item_set.h"

#include <format>
#include <string>

namespace epr {

bool ItemSet::operator==(const ItemSet &rhs) const {
  return items == rhs.items;
}

void ItemSet::push(const Item &item) {
  items.insert(item);
}

std::set<Symbol> ItemSet::next_symbols() const {
  std::set<Symbol> buf;
  for (const auto &item : items)
    if (item.dot_pos < item.rhs.size())
      buf.insert(item.rhs.at(item.dot_pos));
  return buf;
}

void ItemSet::self_closure(const Grammar &grammar) {
  while (true) {
    const usize old_size = items.size();

    for (const auto &item : items) {
      if (item.dot_pos >= item.rhs.size()) {
        continue;
      }

      auto get_production_set = [&](const Symbol &symbol
                                ) -> std::set<std::vector<Symbol>> {
        const auto it = grammar.productions.find(symbol);
        if (it == grammar.productions.end())
          return std::set<std::vector<Symbol>>{};
        return it->second;
      };

      const auto &next_symbol = item.rhs.at(item.dot_pos);
      const auto &production_set = get_production_set(next_symbol);
      for (const auto &first = grammar.first(item.back_slice());
           const Symbol &b : first) {
        for (const auto &production : production_set)
          push({next_symbol, production, 0, b});
      }
    }
    if (items.size() == old_size) // unchanged
      break;
  }
}

ItemSet ItemSet::go(const Grammar &grammar, const Symbol &symbol) const {
  ItemSet buf;
  for (const auto &item : items)
    if (item.dot_pos < item.rhs.size() && item.rhs.at(item.dot_pos) == symbol)
      buf.push(item.advance_dot());
  buf.self_closure(grammar);
  return buf;
}

std::string ItemSet::to_string() const {
  std::string buf;
  auto last_item = items.end();
  for (auto it = items.begin(); it != items.end(); ++it) {
    if (last_item == items.end())
      buf.append(std::format("{}", it->to_string()));
    else if (!it->is_mergeable(*last_item))
      buf.append(std::format("\n{}", it->to_string()));
    else
      buf.append(std::format(" {}", it->lookahead.to_string()));
    last_item = it;
  }
  return buf;
}

} // namespace epr
