#include "parser.h"

ClassAST ClassParser::parse(const std::vector<Token> &) {
  /*
   * typedef class {
   * } name;
   * class name {
   * };
   */
  // ClassAST ast;
  return ClassAST(Token("",Location(0,Column(0,0))));
}

FileAST Parser::parse(const std::vector<Token> &tokens) {
  FileAST file;

  auto it = tokens.begin();
  auto end = tokens.end();
  while (it != end) {
    auto begin = it;
    // const auto &token = *(it)++;

    if (is_preprocessor(begin, end)) {
    } else if (is_class(begin, end)) {
      ClassParser parser;
      auto ast = parser.parse(tokens);
      file.push_back(ast);
    } else if (is_struct(begin, end)) {
    } else if (is_function(begin, end)) {
    }
  }
  return file;
}
