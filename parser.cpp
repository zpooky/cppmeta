#include "parser.h"

ClassAST ClassParser::parse(const std::vector<Token>&){
  return ClassAST{};
}

FileAST Parser::parse(const std::vector<Token> &tokens) {
  FileAST file;
  for (auto &token : tokens) {
    if (token == "class") {
      ClassParser parser;
      auto ast = parser.parse(tokens);
      file.push_back(ast);
    }
    return file;
  }
}
