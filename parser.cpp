#include "parser.h"

FileAST Parser::parse(const std::vector<Token> &tokens) {
  FileAST file;

  auto it = tokens.begin();
  auto end = tokens.end();
  while (it != end) {
    auto begin = it;
    const auto &token = *(it)++;

    if (is_include(begin, end)) {
    } else if (is_define(begin, end)) {
    } else if (is_class(begin, end)) {
      ClassParser parser;
      auto ast = parser.parse(begin, end);
      file.push_back(ast);
    } else if (is_struct(begin, end)) {
    } else if (is_function(begin, end)) {
    }
  }
  return file;
}
