#include "parser.h"
#include "tokenizer.h"
#include <initializer_list>
#include <iostream>

int main() {
  for (auto file :
       {"./tokenizer.cpp", "./tokenizer.h", "./String.h", "./entities.h",
        "./main.cpp", "./parser.h", "./parser.cpp"}) {
    File f(file);
    Tokenizer t(f);
    auto tokens = t.tokenize();

    // for (const auto &token : tokens) {
    //   std::cout << token.to_string() << "\n";
    // }

    Parser parser;
    FileAST ast = parser.parse(tokens);
  }
  return 0;
}

struct CompilationUnit {
  //  \ |  /
  //  file
};
