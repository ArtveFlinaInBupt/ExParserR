#include "parser/grammar.h"

#include "util/all.h"

#include <algorithm>
#include <format>
#include <ranges>

namespace epr {

std::string to_string(const FirstSet &set, const std::string &name) {
  std::string buf;
  for (const auto &[symbol, symbol_set] : set) {
    buf.append(name).append("( ").append(symbol.to_string()).append(" ) = { ");
    if (!symbol_set.empty())
      for (const auto &symbol_ : symbol_set)
        buf.append(symbol_.to_string()).append(", ");
    if (!symbol_set.empty())
      buf.pop_back(), buf.pop_back();
    buf.append(" }\n");
  }
  buf.pop_back();
  return buf;
}

Grammar::Grammar(Symbol start_symbol_):
    start_symbol(std::move(start_symbol_)) {}

Grammar Grammar::from_str(const std::string &str) {
  auto lines = split(str, '\n');
  if (lines.empty())
    throw std::runtime_error("Empty grammar string");

  Grammar grammar(Symbol(lines.front(), Symbol::NonTerminator));

  lines.erase(lines.begin());
  for (auto &&line : lines) {
    auto vec = split(line, " -> ");
    auto lhs = Symbol(vec[0], Symbol::NonTerminator);
    auto rhs_vec = std::vector<std::string>(split(vec[1], " | "));
    auto rhs_set = std::set<std::vector<Symbol>>{};
    for (auto &&rhs : rhs_vec) {
      auto rhs_symbol_vec = std::vector<Symbol>{};
      for (auto &&symbol : split(rhs, ' ')) {
        if (symbol == "ε")
          rhs_symbol_vec.emplace_back(Symbol::empty_symbol());
        else if (isupper(symbol[0]))
          rhs_symbol_vec.emplace_back(symbol, Symbol::NonTerminator);
        else
          rhs_symbol_vec.emplace_back(symbol, Symbol::Terminator);
      }
      rhs_set.emplace(std::move(rhs_symbol_vec));
    }
    grammar.push_productions(lhs, std::move(rhs_set));
  }
  return grammar;
}

Grammar Grammar::from_str(const std::string_view &str) {
  return Grammar::from_str(std::string(str));
}

std::string Grammar::to_string() const {
  const auto [terminators, has_empty_symbol] = get_terminators();
  const auto nonterminators = get_nonterminators();

  std::string buf;

  buf.append(std::format("StartSymbol: {}\n", start_symbol.to_string()));

  buf.append("Terminators: {");
  for (const auto &terminator : terminators)
    buf.append(std::format("{}, ", terminator.to_string()));
  if (!terminators.empty())
    buf.pop_back(), buf.pop_back();
  buf.append("}\n");

  buf.append("NonTerminators: {");
  for (const auto &nonterminator : nonterminators)
    buf.append(std::format("{}, ", nonterminator.to_string()));
  if (!nonterminators.empty())
    buf.pop_back(), buf.pop_back();
  buf.append("}\n");

  buf.append("Productions: {\n");

  for (const auto &[lhs, rhs_set] : productions)
    for (const auto &rhs : rhs_set)
      buf.append(std::format(
          "  ({}) {}\n", production_index.at({lhs, rhs}),
          epr::to_string({lhs, rhs})
      ));
  buf.append("}");
  return buf;
}

std::pair<std::set<Symbol>, bool> Grammar::get_terminators() const {
  std::set<Symbol> terminators{};
  bool has_empty_symbol = false;
  for (const auto &rhs_set : productions | std::views::values)
    for (const auto &rhs : rhs_set)
      for (const auto &symbol : rhs) {
        if (symbol.name.empty())
          has_empty_symbol = true;
        else if (symbol.type == Symbol::Terminator)
          terminators.emplace(symbol);
      }
  return {terminators, has_empty_symbol};
}

std::set<Symbol> Grammar::get_nonterminators() const {
  std::set<Symbol> nonterminators{};
  for (const auto &[lhs, rhs_set] : productions) {
    nonterminators.emplace(lhs);
    for (const auto &rhs : rhs_set)
      for (const auto &symbol : rhs)
        if (symbol.type == Symbol::NonTerminator)
          nonterminators.emplace(symbol);
  }
  return nonterminators;
}

void Grammar::self_augment() {
  auto new_start_symbol =
      Symbol(start_symbol.name + '\'', Symbol::NonTerminator);
  while (productions.contains(new_start_symbol))
    new_start_symbol.name.push_back('\'');
  push_production(new_start_symbol, {start_symbol});
  start_symbol = new_start_symbol;
}

void Grammar::push_productions(
    const Symbol &lhs, const std::set<std::vector<Symbol>> &rhs_set
) {
  productions[lhs].insert(rhs_set.begin(), rhs_set.end());
}

void Grammar::push_productions(
    const Symbol &lhs, std::set<std::vector<Symbol>> &&rhs_set
) {
  productions[lhs].insert(rhs_set.begin(), rhs_set.end());
}

void Grammar::push_production(
    const Symbol &lhs, const std::vector<Symbol> &rhs
) {
  productions[lhs].emplace(rhs);
}

void Grammar::push_production(const Symbol &lhs, std::vector<Symbol> &&rhs) {
  productions[lhs].emplace(std::move(rhs));
}

void Grammar::build_first_set() {
  const auto [terminators, terminators_has_empty] = get_terminators();

  for (const auto &terminator : terminators)
    first_set[terminator].emplace(terminator);
  if (terminators_has_empty)
    first_set[Symbol::empty_symbol()].emplace(Symbol::empty_symbol());

  for (bool changed;;) {
    changed = false;

    for (const auto &[lhs, rhs_set] : productions) {
      for (const auto &rhs : rhs_set) {
        if (rhs.empty()) {
          changed |= first_set[lhs].emplace(Symbol::empty_symbol()).second;
          continue;
        }

        const auto old_size = first_set[lhs].size();
        for (const auto &symbol : rhs) {
          auto &first_set_rhs = first_set[symbol];
          first_set[lhs].insert(first_set_rhs.begin(), first_set_rhs.end());
          if (!first_set_rhs.contains(Symbol::empty_symbol()))
            break;
        }
        changed |= first_set[lhs].size() != old_size;
      }
    }

    if (!changed)
      break;
  }
  first_set[epr::Grammar::END_SYMBOL].emplace(epr::Grammar::END_SYMBOL);
}

void Grammar::build_production_index() {
  for (const auto &[lhs, rhs_set] : productions)
    for (const auto &rhs : rhs_set) {
      production_list.emplace_back(lhs, rhs);
      production_index[{lhs, rhs}] = production_list.size() - 1;
    }
}

std::set<Symbol> Grammar::first(const std::vector<Symbol> &str) const {
  std::set<Symbol> ret{};
  for (const auto &symbol : str) {
    const auto &first_set_rhs = first_set.at(symbol);
    ret.insert(first_set_rhs.begin(), first_set_rhs.end());
    if (!first_set_rhs.contains(Symbol::empty_symbol()))
      break;
  }
  return ret;
}

} // namespace epr
