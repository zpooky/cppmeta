#include "String.h"
#include "parser.h"
#include "preprocessor.h"
#include "tokenizer.h"
#include <initializer_list>
#include <iostream>

using Iterator = sp::ArrayList<Token>::const_iterator;

int main() {
  std::vector<String> files = {"./tokenizer.cpp", "./tokenizer.h", "./String.h",
                               "./entities.h",    "./main.cpp",    "./parser.h",
                               "./parser.cpp"};

  std::vector<String> tests = {"./test/Test1cpp"};

  // sp::ArrayList<int> a;
  // uint64_t d(0);
  // for (int i = 0; i < 5000; ++i) {
  //   for (const auto &c : a) {
  //     d += c;
  //   }
  //   a.push_back(i);
  // }
  // printf("%lu\n",d);

  for (auto file : tests) {
    try {
      File f(file);
      Tokenizer t(f);
      auto tokens = t.tokenize();

      ast::Preprocessor pre;
      ast::FileAST fileAST;
      auto ppTokens = pre.parse(tokens, fileAST);

      std::cout << "\n\n####" << f.name.c_str() << "\n\n";
      for (const auto &token : ppTokens) {
        std::cout << token.to_string() << "\n";
      }
      std::cout << "\n";

      match::Step<Iterator> start(ppTokens.begin(), ppTokens.end());
      ast::Parser parser;
      auto response = parser.parse(fileAST, start);
      if (!response.valid) {
        std::cerr << "invalid error:" << (*response.it).token.c_str() << "\n";
        exit(1);
      }

      if (response.it != response.end) {
        std::cerr << "not end error:" << (*response.it).to_string() << "\n";
        exit(1);
      }

      auto current = fileAST.to_yaml();
      std::cout << current.to_string() << "\n";

    } catch (const Token &t) {
      std::cerr << "error:" << t.to_string() << "\n";
    }
  } //
  return 0;
}
