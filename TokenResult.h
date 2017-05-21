#ifndef SP_META_CPP_TOKEN_RESULT_H
#define SP_META_CPP_TOKEN_RESULT_H

#include "tokens.h"
#include "LineMeta.h"
#include <vector>

/**/
struct TokenResult {
  LineMeta &line;
  Location location;
  std::vector<Token> &result;

  TokenResult(std::vector<Token> &p_result, LineMeta &p_line)
      : line(p_line), location(0, Column(0, 0)), result(p_result) {
  }

  void push_back(String &token) {
    if (!token.is_empty()) {
      location.column.end = token.length();
      result.emplace_back(token, location);
      token.clear();
    }
    location = line.loc();
  }
};

#endif
