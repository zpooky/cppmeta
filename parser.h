#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "entities.h"
#include <vector>
#include "ast.h"

namespace {
class ClassParser {
  public:
  ClassAST parse(const std::vector<Token>&);
};

}

class Parser {
public:
  Parser() {
  }

  FileAST parse(const std::vector<Token> &);
};

#endif
