#include "parser.h"

ClassAST ClassParser::parse(std::vector<Token>&){
  return ClassAST{};
}

FileAST Parser::parse(std::vector<Token> &tokens) {
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
