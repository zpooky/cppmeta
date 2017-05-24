#include "parser.h"
#include "ClassParser.h"

namespace ast {

FileAST Parser::parse(const std::vector<Token> &tokens) {
  using Iterator = std::vector<Token>::const_iterator;
  FileAST file;

  Iterator it = tokens.begin();
  Iterator end = tokens.end();
  while (it != end) {
    auto begin = it++;
    // const auto &token = *(it)++;

    {
      ClassParser<Iterator> parser;
      ClassAST ast;
      match::Step<Iterator> next = parser.parse(begin, end, ast);
      if (next.valid) {
        file.push_back(ast);
        it = next;
        continue;
      }
    }
    // } else if (is_struct(begin, end)) {
    //
    // } else if (is_function_declaration(begin, end)) {
    //   // } else if (is_function_definition(begin, end)) {
    //
    //   // } else if (is_ctor_declaration(begin, end)) {
    //   // } else if (is_ctor_definition(begin, end)) {
    //   //   // TODO based on context
    //   //   // BaseTokenizer(){}
    //   //   // BaseTokenizer::BaseTokenizer(){}
    //   //
    //   // } else if (is_dtor_declaration(begin, end)) {
    //   // } else if (is_dtor_definition(begin, end)) {
    //   //   // TODO based on context
    //   //   // ~BaseTokenizer(){}
    //   //   // ~BaseTokenizer::BaseTokenizer(){}
    //   //
    //   // } else if (is_operator_declaration(begin, end)) {
    //   // } else if (is_operator_definition(begin, end)) {
    // }
    exit(1);
  }
  return file;
} // Parse::parse

} // namespace ast
