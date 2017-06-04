#include "String.h"
#include "parser.h"
#include "preprocessor.h"
#include "tokenizer.h"
#include <initializer_list>
#include <iostream>

int main() {
  std::vector<String> files = {"./tokenizer.cpp", "./tokenizer.h", "./String.h",
                               "./entities.h",    "./main.cpp",    "./parser.h",
                               "./parser.cpp"};

  std::vector<String> tests = {"./test/Test1cpp"};

  sp::ArrayList<int> a;
  for (int i = 0; i < 5000; ++i) {
    a.push_back(i);
  }
  for (const auto &c : a) {
    printf("%d\n", c);
  }

  // for (auto file : tests) {
  //   try {
  //     File f(file);
  //     Tokenizer t(f);
  //     auto tokens = t.tokenize();
  //
  //     ast::Preprocessor pre;
  //     ast::FileAST fileAST;
  //     auto ppTokens = pre.parse(tokens, fileAST);
  //
  //     std::cout << "\n\n####" << f.name.c_str() << "\n\n";
  //     // for (const auto &token : tokens) {
  //     // std::cout << token.to_string() << "\n";
  //     // }
  //     std::cout << "\n";
  //
  //     ast::Parser parser;
  //     parser.parse(ppTokens, fileAST);
  //
  //     auto current = fileAST.to_yaml();
  //     std::cout << current.to_string() << "\n";
  //
  //   } catch (const Token &t) {
  //     std::cerr << "error:" << t.to_string() << "\n";
  //   }
  // } //
  return 0;
}

struct CompilationUnit {
  //  \ |  /
  //  file
};
