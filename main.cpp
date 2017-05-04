#include "parser.h"
#include "tokenizer.h"
#include <iostream>

int main() {
  Tokenizer t(File("./tokenizer.cpp"));
  auto tokens = t.tokenize();

  for (const auto &token : tokens) {
    std::cout << token.to_string() << "\n";
  }

  Parser parser;
  FileAST ast = parser.parse(tokens);
  return 0;
}
