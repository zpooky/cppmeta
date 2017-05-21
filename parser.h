#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ast.h"
#include "tokens.h"
#include "matcher.h"
#include <vector>

namespace ast {

template <typename It>
bool is_tokens(It, It);

template <typename It, typename Head, typename... Tails>
bool is_tokens(It, It, const Head &, const Tails &...);

template <typename It>
bool is_tokens(It, It) {
  return true;
}

template <typename It, typename Head, typename... Tails>
bool is_tokens(It begin, It end, const Head &token, const Tails &... tail) {
  bool valid(false);
  if (begin != end) {
    valid = *begin == token;
  }
  return valid && is_tokens(begin + 1, end, tail...);
}

template <typename Iterator>
bool is_class(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "class");
}

template <typename Iterator>
bool is_include(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "#", "include");
}

template <typename Iterator>
bool is_define(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "#", "define");
}

template <typename Iterator>
bool is_struct(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "typedef", "struct") ||
         is_tokens(begin, end, "struct");
}

template <typename Iterator>
bool is_function_declaration(Iterator it, Iterator end) {
  return false;
}

class Parser {
public:
  Parser() {
  }

  FileAST parse(const std::vector<Token> &);
};
}

#endif
