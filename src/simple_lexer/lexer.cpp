#include "simple_lexer/lexer.h"

#include <cctype>
#include <set>

namespace epr {

Lexer Lexer::with_src(const std::string &src) {
  Lexer lexer{};
  lexer.load_src(src);
  return lexer;
}

void Lexer::load_src(const std::string &src) {
  pos_ = 0;
  src_ = src;
}

void Lexer::load_src(std::string &&src) {
  pos_ = 0;
  src_ = std::move(src);
}

TokenStream Lexer::lex_effective() {
  TokenStream token_stream;
  for (std::optional<Token> token_opt; (token_opt = next_token());) {
    std::visit(
        overloaded{
            [](const LexError &) {
              throw std::runtime_error("Lex error");
            },
            [](const Whitespace &) {},
            [&](const auto &token) {
              token_stream.push_back(token);
            },
        },
        *token_opt
    );
  }
  return token_stream;
}

std::optional<char> Lexer::peek(const usize offset) const {
  if (pos_ + offset >= src_.size())
    return std::nullopt;
  return src_.at(pos_ + offset);
}

bool Lexer::reached_eof() const {
  return pos_ >= src_.size();
}

std::optional<char> Lexer::consume() {
  if (reached_eof())
    return std::nullopt;
  return src_.at(pos_++);
}

std::optional<Token> Lexer::next_token() {
  const auto cur_char = consume();
  if (!cur_char)
    return std::nullopt;

  if (isspace(*cur_char))
    return consume_whitespace();
  if (isdigit(*cur_char))
    return consume_integer();
  return punctuator(*cur_char);
}

Token Lexer::consume_whitespace() {
  while (peek() && isspace(*peek()))
    consume();
  return Whitespace{};
}

Token Lexer::consume_integer() {
  while (peek() && isdigit(*peek()))
    consume();
  return Integer{};
}

Token Lexer::punctuator(const char first_char) {
  static std::set<char> valid_punctuators{'(', ')', '+', '-', '*', '/'};
  if (valid_punctuators.contains(first_char))
    return Punctuator{first_char};
  return LexError{};
}

} // namespace epr
