#pragma once

#ifndef EPR_SIMPLE_LEXER_TOKEN_H
#  define EPR_SIMPLE_LEXER_TOKEN_H

#  include <variant>
#  include <vector>

namespace epr {

struct Integer {};

struct Punctuator {
  char punct{};

  explicit Punctuator(const char punct): punct(punct) {}
};

struct Whitespace {};

struct LexError {};

using Token = std::variant<Integer, Punctuator, Whitespace, LexError>;

using TokenStream = std::vector<Token>;

} // namespace epr

#endif // !EPR_SIMPLE_LEXER_TOKEN_H
