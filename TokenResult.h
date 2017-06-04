#ifndef SP_META_CPP_TOKEN_RESULT_H
#define SP_META_CPP_TOKEN_RESULT_H

#include "ArrayList.h"
#include "LineMeta.h"
#include "tokens.h"

/**/
struct TokenResult {
  LineMeta &line;
  Location location;
  sp::ArrayList<Token> &result;

  TokenResult(sp::ArrayList<Token> &p_result, LineMeta &p_line)
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
