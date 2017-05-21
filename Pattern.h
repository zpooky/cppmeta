#ifndef SP_CPP_META_CLASS_PATTERN_H
#define SP_CPP_META_CLASS_PATTERN_H

#include "ast.h"
#include "lang.h"
#include "matcher.h"

namespace ast {

struct Pattern {
  template <typename Iterator>
  match::Step<Iterator> operator()(Token &, match::Step<Iterator> it) {
    if (it.valid) {
      if (it.it != it.end) {
        for (const char *keyword : cpp::keywords) {
          if (*it.it == keyword) {
            return match::Step<Iterator>(it.it + 1, it.end, false);
          }
        }
      }
    }
    // TODO
    auto next = it.it != it.end ? it.it + 1 : it.it;
    return match::Step<Iterator>(next, it.end, true);
  }
};

Pattern TypeName();
Pattern VariableName();

struct TypeIdentifierParser {

  template <typename Iterator>
  match::Step<Iterator> operator()(TypeIdentifier &result,
                                   match::Step<Iterator> it) {
    Pattern p;
    Token t;
    auto next = p(t, it);
    if (next) {
      result = TypeIdentifier(t, {});
    }
    return it;
  }
};
}

#endif
