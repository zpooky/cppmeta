#include "String.h"
#include "parser.h"
#include "tokenizer.h"
#include <initializer_list>
#include <iostream>

int main() {
  std::vector<String> files = {"./tokenizer.cpp", "./tokenizer.h", "./String.h",
                               "./entities.h",    "./main.cpp",    "./parser.h",
                               "./parser.cpp"};

  std::vector<String> tests = {"./test/Test1.cpp"};

  for (auto file : tests) {
    File f(file);
    Tokenizer t(f);
    auto tokens = t.tokenize();

    // std::cout << "\n\n####" << f.name.c_str() << "\n\n";
    // for (const auto &token : tokens) {
    //   std::cout << token.to_string() << "\n";
    // }
    // std::cout << "\n";

    ast::Parser parser;
    ast::FileAST ast = parser.parse(tokens);
    for (auto &c : ast.classes) {
      // std::cout << yaml::List(c.inherits).to_string("") << "\n";
      auto current = c.to_yaml();
      std::cout << current.to_string();
    }
  }
  return 0;
}

struct CompilationUnit {
  //  \ |  /
  //  file
};
