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

    } else if (is_function_declaration(begin, end)) {
    // } else if (is_function_definition(begin, end)) {

    // } else if (is_ctor_declaration(begin, end)) {
    // } else if (is_ctor_definition(begin, end)) {
    //   // TODO based on context
    //   // BaseTokenizer(){}
    //   // BaseTokenizer::BaseTokenizer(){}
    //
    // } else if (is_dtor_declaration(begin, end)) {
    // } else if (is_dtor_definition(begin, end)) {
    //   // TODO based on context
    //   // ~BaseTokenizer(){}
    //   // ~BaseTokenizer::BaseTokenizer(){}
    //
    // } else if (is_operator_declaration(begin, end)) {
    // } else if (is_operator_definition(begin, end)) {
    }
  }
  return file;
}
