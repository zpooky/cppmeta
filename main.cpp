#include "StripComments.h"
#include "parser.h"
#include "tokenizer.h"
#include <iostream>

int main() {
  Tokenizer t(File("./tokenizer.cpp"));
  auto tokens = t.tokenize();

  for (const auto &token : tokens) {
    std::cout << token.to_string() << "\n";
  }

  StripComments scomment;
  auto strippedTokens = scomment.parse(tokens);

  Parser parser;
  FileAST ast = parser.parse(strippedTokens);
  return 0;
}
