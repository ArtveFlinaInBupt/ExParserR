#pragma once

#ifndef EPR_PARSER_PARSER_H
#  define EPR_PARSER_PARSER_H

#  include "dfa.h"
#  include "parser/symbol.h"
#  include "simple_lexer/lexer.h"
#  include "util/all.h"

#  include <map>
#  include <variant>

namespace epr {

using SymbolStream = std::vector<Symbol>;

struct Error {};

struct Shift {
  usize state{};
};

struct Goto {
  usize state{};
};

struct Reduce {
  usize rule{};
};

struct Accept {};

using Action = std::variant<Error, Shift, Goto, Reduce, Accept>;

std::string to_string(const Action &action);

struct ParsingTable {
  std::map<Symbol, usize> terminals{};
  std::map<Symbol, usize> non_terminals{};
  std::vector<std::vector<Action>> table{};

  ParsingTable() = default;

  ParsingTable(const Dfa &dfa, const Grammar &grammar);

  [[nodiscard]] Action get_action(usize state, const Symbol &symbol) const;

  [[nodiscard]] Table to_table() const;

  [[nodiscard]] std::string to_string() const;
};

using OutputEntry = std::vector<std::string>;

OutputEntry to_output_entry(
    const std::vector<usize> &stack, const std::vector<Symbol> &symbols,
    const std::vector<Symbol> &input, usize input_idx, const std::string &action
);

struct Parser {
  Grammar grammar_{Grammar::END_SYMBOL};
  ParsingTable table{};

  explicit Parser(Grammar grammar);

  static SymbolStream tokens_to_symbols(std::vector<Token> &&token_stream);

  std::vector<OutputEntry> parse_expr(SymbolStream &&input);

  void parse_src(const std::string &src);

  [[nodiscard]] std::string action_str(const Action &action) const;
};

} // namespace epr

#endif // !EPR_PARSER_PARSER_H
