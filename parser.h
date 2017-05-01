#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ast.h"
#include "entities.h"
#include <vector>

namespace {
class ClassParser {
public:
  template <typename Iterator>
  ClassAST parse(Iterator it, Iterator end) {
    /*
     * typedef class {
     * } name;
     * class name {
     * };
     */
    // ClassAST ast;
    return ClassAST(Token("", Location(0, Column(0, 0))));
  }
};

template <typename Iterator>
bool is_class(Iterator it, Iterator end) {
  const auto &token = *it;

  if (token == "class") {
    return true;
  }
  return false;
}

template <typename Iterator>
bool is_preprocessor(Iterator it, Iterator end) {
  const auto &token = *it;

  if (token.token.starts_with("#")) {
    return true;
  }
  return false;
}

template <typename Iterator>
bool is_struct(Iterator it, Iterator end) {
  return false;
}

template <typename Iterator>
bool is_function(Iterator it, Iterator end) {
  return false;
}
}

class Parser {
public:
  Parser() {
  }

  FileAST parse(const std::vector<Token> &);
};

#endif
