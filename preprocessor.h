#ifndef SP_CPP_META_PREPROCESSOR_H
#define SP_CPP_META_PREPROCESSOR_H

#include "ast.h"
#include "matcher.h"
#include <iostream>
#include <vector>
//

namespace ast {
class Preprocessor {
private:
  using Tokens = std::vector<Token>;
  using StepType = match::Step<Tokens::const_iterator>;

  template <typename...>
  StepType match(StepType);

  template <typename... Tail>
  StepType match(StepType, String, Tail...);

  template <typename... Tail>
  StepType match(StepType, Token &, Tail...);

  // template <typename... Tail>
  // StepType match(StepType, Tail...);

  template <typename Result, typename Function>
  void for_each(Result &res, StepType s, Function f) {
    StepType current = s;
  start:
    if (current.it != current.end) {
      StepType result = f(current);
      if (result.valid) {
        current = result;
      } else {
        res.push_back(*current.it);
        current = StepType(current.it + 1, current.end, true);
      }
      goto start;
    }
  }

public:
  Preprocessor() {
  }

  Tokens parse(Tokens &tokens, FileAST &result) {
    Tokens res;
    StepType start(tokens.begin(), tokens.end(), true);
    for_each(res, start, [&](StepType current) -> StepType {
      Token t;
      auto r = match::either(current,
                             [&](StepType it) -> StepType {
                               //
                               return match(it, "#", "include", "<", t, ">");
                               // return
                               // it.step("#").step("include").step("<").step(t).step(">");
                             },
                             [&](StepType it) -> StepType {
                               //
                               // return
                               // it.step("#").step("include").step("\"").step(t).step("\"");
                               return match(it, "#", "include", "\"", t, "\"");
                             });
      if (r) {
        result.push_back(IncludeAST());
      }
      return r;
    });
    return res;
  }
};

template <typename...>
Preprocessor::StepType Preprocessor::match(StepType s) {
  return s;
}
template <typename... Tail>
Preprocessor::StepType Preprocessor::match(StepType s, String constant,
                                           Tail... t) {
  return match(s.step(constant), t...);
}

template <typename... Tail>
Preprocessor::StepType Preprocessor::match(StepType s, Token &capture,
                                           Tail... t) {
  return match(s.step(capture), t...);
}

// template <typename... Tail>
// Preprocessor::StepType Preprocessor::match(StepType s, Tail... t) {
//   return match(s, t...);
// }

} // namespace ast
#endif
