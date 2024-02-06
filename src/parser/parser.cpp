#include "parser/parser.h"

#include "parser/dfa.h"

#include <format>
#include <iostream>
#include <utility>

namespace epr {

using namespace std::string_literals;

std::string to_string(const Action &action) {
  return std::visit(
      overloaded{
          [](const Error &) {
            return ""s;
          },
          [](const Shift &shift) {
            return std::format("s{}", shift.state);
          },
          [](const Goto &go_to) {
            return std::format("{}", go_to.state);
          },
          [](const Reduce &reduce) {
            return std::format("r{}", reduce.rule);
          },
          [](const Accept &) {
            return "acc"s;
          },
      },
      action
  );
}

ParsingTable::ParsingTable(const Dfa &dfa, const Grammar &grammar) {
  usize col_idx = 0;
  for (const auto &symbol : grammar.get_terminators().first)
    terminals.insert({symbol, ++col_idx});
  terminals.insert({Grammar::END_SYMBOL, ++col_idx});
  for (const auto &symbol : grammar.get_nonterminators())
    non_terminals.insert({symbol, ++col_idx});

  usize row_idx = 0;
  table.resize(dfa.states.size());
  for (const auto &state : dfa.states) {
    table[row_idx].resize(col_idx + 1);

    // accept and reduce
    for (const auto &item : state.items)
      if (item.dot_pos == item.rhs.size()) {
        if (item.lhs == grammar.start_symbol)
          table[row_idx][terminals.at(Grammar::END_SYMBOL)] = Action{Accept{}};
        else
          table[row_idx][terminals.at(item.lookahead)] =
              Action{Reduce{grammar.production_index.at({item.lhs, item.rhs})}};
      }

    // shift and goto
    for (const auto &[symbol, next_state_idx] : dfa.transitions.at(row_idx))
      if (symbol.type == Symbol::Type::Terminator)
        table[row_idx][terminals.at(symbol)] = Action{Shift{next_state_idx}};
      else
        table[row_idx][non_terminals.at(symbol)] = Action{Goto{next_state_idx}};

    ++row_idx;
  }
}

Action ParsingTable::get_action(const usize state, const Symbol &symbol) const {
  switch (symbol.type) {
    case Symbol::Type::Terminator:
      return table.at(state).at(terminals.at(symbol));
    case Symbol::Type::NonTerminator:
      return table.at(state).at(non_terminals.at(symbol));
    default:
      std::unreachable();
  }
}

Table ParsingTable::to_table() const {
  Table ret;
  ret.resize(table.size() + 1);

  { // header
    ret[0].resize(terminals.size() + non_terminals.size() + 1);
    for (const auto &[symbol, idx] : terminals)
      ret[0][idx] = symbol.to_string();
    for (const auto &[symbol, idx] : non_terminals)
      ret[0][idx] = symbol.to_string();
  }

  // body
  for (usize i = 0; i < table.size(); ++i) {
    ret[i + 1].resize(table.at(i).size());
    ret[i + 1][0] = std::format("I{}", i);
    for (usize j = 1; j < table.at(i).size(); ++j)
      ret[i + 1][j] = epr::to_string(table.at(i).at(j));
  }

  return ret;
}

std::string ParsingTable::to_string() const {
  return epr::to_string(to_table(), [](const usize x, usize) {
    return x == 0 ? Align::Center : Align::Left;
  });
}

OutputEntry to_output_entry(
    const std::vector<usize> &stack, const std::vector<Symbol> &symbols,
    const std::vector<Symbol> &input, const usize input_idx,
    const std::string &action
) {
  std::string stack_str;
  for (const auto &x : stack)
    stack_str.append(std::to_string(x)).append(1, ' ');

  std::string symbols_str;
  for (const auto &x : symbols)
    symbols_str.append(x.to_string());

  std::string input_str;
  for (usize i = input_idx; i < input.size(); ++i)
    input_str.append(input.at(i).to_string());

  return {stack_str, symbols_str, input_str, action};
}

Parser::Parser(Grammar grammar) {
  grammar.self_augment();
  grammar.build_production_index();
  std::cout << std::format(
      "\033[1;32m==== Augmented Grammar ====\033[0m\n{}\n", grammar.to_string()
  );
  std::cout << std::endl;

  grammar.build_first_set();
  std::cout << std::format(
      "\033[1;32m==== FIRST Set ====\033[0m\n{}\n",
      to_string(grammar.first_set, "FIRST")
  );
  std::cout << std::endl;

  const auto dfa = Dfa(grammar);
  std::cout << std::format(
      "\033[1;32m==== LR(1) Sets of Items ==== \033[0m\n{}\n",
      dfa.sets_to_string()
  );
  std::cout << std::endl;
  std::cout << std::format(
      "\033[1;32m==== LR(1) DFA ==== \033[0m\n{}\n", dfa.transitions_to_string()
  );

  grammar_ = grammar;
  table = ParsingTable(dfa, grammar);
  std::cout << std::format(
      "\033[1;32m==== Parsing Table ==== \033[0m\n{}\n", table.to_string()
  );
  std::cout << std::endl;
}

SymbolStream Parser::tokens_to_symbols(std::vector<Token> &&token_stream) {
  SymbolStream buf;
  for (const auto &token : token_stream) {
    std::visit(
        overloaded{
            [&](const Integer &) {
              buf.emplace_back("n", Symbol::Terminator);
            },
            [&](const Punctuator &punct) {
              buf.emplace_back(std::string(1, punct.punct), Symbol::Terminator);
            },
            [](const auto &) {},
        },
        token
    );
  }
  return buf;
}

std::vector<OutputEntry> Parser::parse_expr(SymbolStream &&input) {
  input.emplace_back(Grammar::END_SYMBOL);
  std::vector<OutputEntry> buf = {
      {"Stack", "Symbols", "Input", "Action"}
  };

  std::vector<usize> stack{0};
  std::vector<Symbol> symbols{};
  bool has_error = false;

  for (usize input_idx = 0;;) {
    const auto &cur_state = stack.back();
    const auto &cur_symbol = input.at(input_idx);
    const auto &action = table.get_action(cur_state, cur_symbol);

    buf.push_back(
        to_output_entry(stack, symbols, input, input_idx, action_str(action))
    );

    if (std::holds_alternative<Accept>(action))
      break;
    bool sync = false;

    std::visit(
        overloaded{
            [&](const Shift &shift) {
              stack.push_back(shift.state);
              symbols.push_back(cur_symbol);
              ++input_idx;
            },

            [&](const Reduce &reduce) {
              const auto &[lhs, rhs] = grammar_.production_list.at(reduce.rule);

              for (usize _ = 0; _ < rhs.size(); ++_) {
                stack.pop_back();
                symbols.pop_back();
              }

              const auto &cur_top_state = stack.back();
              const auto &next_state = table.get_action(cur_top_state, lhs);
              stack.push_back(std::get<Goto>(next_state).state);
              symbols.push_back(lhs);
            },

            [&](const Error &) {
              has_error = true;
              while (true) {
                if (stack.empty() || symbols.empty())
                  break;
                if (!std::holds_alternative<Error>(
                        table.get_action(stack.back(), cur_symbol)
                    ))
                  break;
                stack.pop_back();
                symbols.pop_back();
                sync = true;
              }
            },

            [&](const auto &) {
              std::unreachable();
            },
        },
        action
    );

    if (std::holds_alternative<Error>(action) && !sync)
      break;
  }

  if (has_error)
    buf.back().back() = "Finish [ERROR OCCURRED]";

  return buf;
}

void Parser::parse_src(const std::string &src) {
  auto lexer = Lexer::with_src(src);
  auto tokens = lexer.lex_effective();
  auto symbols = tokens_to_symbols(std::move(tokens));

  std::cout << "\033[1;32m==== Token Stream ====\033[0m\n";
  for (const auto &symbol : symbols)
    std::cout << symbol.to_string() << " ";
  std::cout << std::endl << std::endl;

  const auto table = parse_expr(std::move(symbols));

  std::cout << std::format(
      "\033[1;32m==== Parsing procedure ====\033[0m\n{}\n",
      to_string(
          table,
          [](const usize x, const usize y) {
            if (x == 0)
              return Align::Center;
            if (y == 2)
              return Align::Right;
            return Align::Left;
          }
      )
  );
  std::cout << std::endl;
}

std::string Parser::action_str(const Action &action) const {
  return std::visit(
      overloaded{
          [](const Error &) {
            return "Error"s;
          },
          [](const Shift &shift) {
            return std::format("Shift {}", shift.state);
          },
          [&](const Reduce &reduce) {
            return std::format(
                "Reduce {}", to_string(grammar_.production_list.at(reduce.rule))
            );
          },
          [](const Accept &) {
            return "Accept"s;
          },
          [](const auto &) -> std::string {
            std::unreachable();
          }
      },
      action
  );
}

} // namespace epr
