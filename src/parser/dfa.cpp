#include "parser/dfa.h"

#include "util/all.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <string>

namespace epr {

Dfa::Dfa(Grammar &grammar) {
  State start_state;
  const auto &start_production_set =
      grammar.productions.at(grammar.start_symbol);
  assert(start_production_set.size() == 1); // ensures that the grammar is
                                            // augmented
  const auto &start_production = *start_production_set.begin();
  start_state.push(
      {grammar.start_symbol, start_production, 0, Grammar::END_SYMBOL}
  );
  start_state.self_closure(grammar);
  states.push_back(start_state);
  transitions.resize(1);

  for (usize idx = 0; idx < states.size(); ++idx)
    for (const auto &symbol : states.at(idx).next_symbols()) {
      const auto &&next_state = states.at(idx).go(grammar, symbol);
      if (next_state.items.empty())
        continue;
      auto it = std::ranges::find(states, next_state);
      if (it == states.end()) {
        states.push_back(next_state);
        transitions.resize(states.size());
        it = std::prev(states.end());
      }
      transitions.at(idx).insert({symbol, std::distance(states.begin(), it)});
    }
}

std::string Dfa::sets_to_string() const {
  std::string buf;
  for (usize idx = 0; idx < states.size(); ++idx)
    buf.append(std::format(
        "\033[33mI{}:\033[0m\n{}\n\n", idx, states.at(idx).to_string()
    ));
  return buf;
}

std::string Dfa::transitions_to_string() const {
  std::string buf;
  for (usize idx = 0; idx < states.size(); ++idx) {
    if (transitions.at(idx).empty())
      continue;
    bool first = true;
    for (const auto &[symbol, next_state_idx] : transitions.at(idx)) {
      if (first)
        buf.append(std::format("I{}", idx));
      else
        buf.append(std::format("{}", std::string(1 + len(idx), ' ')));
      buf.append(
          std::format(" --- {} --> I{}\n", symbol.to_string(), next_state_idx)
      );
      // buf.append(std::format(
      //     "I{} -> I{} [label = \"{}\"]\n", idx, next_state_idx,
      //     symbol.to_string()
      // ));
      first = false;
    }
    buf.append("\n");
  }
  return buf;
}

} // namespace epr
