#ifndef SP_META_CPP_AST_ENTETIES_H
#define SP_META_CPP_AST_ENTETIES_H

#include "String.h"
#include "entities.h"

namespace match {
template <typename Iterator>
struct Step {
  using SelfType = Step<Iterator>;
  Iterator it;
  Iterator end;
  bool valid;

  Step(Iterator p_it, Iterator p_end, bool p_valid)
      : it(p_it), end(p_end), valid(p_valid) {
  }

  Step(Iterator p_it, Iterator p_end) : Step(p_it, p_end, true) {
  }

  SelfType step(const String &constant) {
    if (valid && it != end) {
      if (*it == constant) {
        return Step(it + 1, end, true);
      }
    }
    return Step(it, end, false);
  }

  SelfType step(Token &capture) {
    if (valid && it != end) {
      capture = *it;
      return Step(it + 1, end, true);
    }
    return Step(it, end, false);
  }

  operator Iterator() {
    return it;
  }

  operator bool() {
    return valid;
  }
};

template <typename Iterator>
Step<Iterator> start(Iterator begin, Iterator end) {
  return Step<Iterator>(begin, end);
}

}

#endif
