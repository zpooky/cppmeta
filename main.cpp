#include "tokenizer.h"
#include "parser.h"

int main(){
  Tokenizer t(File("./tokenizer.cpp"));
  auto result = t.tokenize();

  Parser parser;
  FileAST ast = parser.parse(result);
  // for(auto& token : result){
  // }
  return 0;
}
