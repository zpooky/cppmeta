#ifndef SP_CPP_META_CLASS_PATTERN_H
#define SP_CPP_META_CLASS_PATTERN_H

#include "ast.h"
#include "lang.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
struct Pattern : public match::Base<Token, Iterator> {
  match::Step<Iterator> operator()(Token &t, match::Step<Iterator> it) const {
    if (it.valid) {
      if (it.it != it.end) {
        for (const char *keyword : cpp::keywords) {
          if (*it.it == keyword) {
            return match::Step<Iterator>(it.it, it.end, false);
          }
        }
        //
        for (const char *s : cpp::symbols) {
          if (*it.it == s) {
            return match::Step<Iterator>(it.it, it.end, false);
          }
        }
      }
    }

    t = *it.it;

    auto next = it.it != it.end ? it.it + 1 : it.it;
    return match::Step<Iterator>(next, it.end, true);
  }
};

template <typename Iterator>
Pattern<Iterator> TypeName() {
  return {};
}
template <typename Iterator>
Pattern<Iterator> VariableName() {
  return {};
}

template <typename Iterator>
struct TypeIdentifierParser : public match::Base<TypeIdentifier, Iterator> {

  match::Step<Iterator> operator()(TypeIdentifier &result,
                                   match::Step<Iterator> it) const {
    Pattern<Iterator> p;
    Token t;
    auto next = p(t, it);
    if (next) {
      result = TypeIdentifier(t, {});
    }
    return next;
  }
};
}

#endif
