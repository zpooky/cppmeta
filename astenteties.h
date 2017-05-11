#ifndef SP_META_CPP_AST_ENTETIES_H
#define SP_META_CPP_AST_ENTETIES_H

#include "String.h"
#include "entities.h"
#include <utility>
#include <vector>

namespace match {

struct Pattern {
  bool match(const Token &match) const {
    // TODO
    return true;
  }
};

struct Either {
  std::vector<String> constants;

  template <typename... Tail>
  Either(Tail &&... tail) : constants{std::forward<String>(tail)...} {
  }

  bool match(const Token &match) const {
    for (const auto &current : constants) {
      if (match == current) {
        return true;
      }
    }
    return false;
  }
};

template <typename... Tail>
Either either(const Tail &... tail);

template <typename... Tail>
Either either(Tail... tail) {
  return Either(std::forward<String>(tail)...);
}

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
    if (valid) {
      if (it != end) {
        if (*it == constant) {
          return Step(it + 1, end, true);
        }
      }
    }
    return Step(it, end, false);
  }

  SelfType step(Token &capture) {
    if (valid) {
      if (it != end) {
        capture = *it;
        return Step(it + 1, end, true);
      }
    }
    return Step(it, end, false);
  }

  SelfType step(Token &token, const Either &e) {
    if (valid) {
      if (it != end) {
        if (e.match(*it)) {
          token = *it;
          return Step(it + 1, end, true);
        }
      }
    }
    return Step(it, end, false);
  }

  SelfType step(Token &token, const Pattern &p) {
    if (valid) {
      if (it != end) {
        if (p.match(*it)) {
          token = *it;
          return Step(it + 1, end, true);
        }
      }
    }
    return Step(it, end, false);
  }

  SelfType option(const String &) {
    // TODO
    return SelfType(it, end, valid);
  }

private:
  SelfType either(Token &match) {
    return SelfType(it, end, false);
  }

public:
  template <typename Constant, typename... Constants>
  SelfType either(Token &match, const Constant &head,
                  const Constants &... tail) {
    if (it != end && valid) {
      if (*it == head) {
        match = *it;
        return SelfType(it + 1, end, true);
      }
      return either(match, tail...);
    } else {
      return SelfType(it, end, false);
    }
  }

  // template <typename... Term>
  // SelfType combination(Token &, const String &, const Term &...);
  //
  // template <typename... Term>
  // SelfType combination(Token &, const Either &, const Term &...);

private:
  SelfType combination(Token &match) {
    return SelfType(it, end, valid);
  }

public:
  template <typename... Term>
  SelfType combination(std::vector<Token> &match, const String &c,
                       const Term &... terms) {
    return SelfType(it, end, valid);
  }

  template <typename... Term>
  SelfType combination(std::vector<Token> &match, const Either &e,
                       const Term &... terms) {
    return SelfType(it, end, valid);
  }

  template <typename Function>
  SelfType repeat(Function f, const String &separator) {
    SelfType current = *this;
  start:
    SelfType result = f(current);
    if (result) {
      current = result;
      if (current.it != current.end && *current.it == separator) {
        current = SelfType(current.it + 1, current.end, true);
        goto start;
      }
    }
    return current;
  }

  operator Iterator() {
    return it;
  }

  operator bool() {
    return valid;
  }
};

// Step<Iterator> either(Token&);
// Step<Iterator> either(Token&){
// }

// template<typename Constant,typename ...Constants>
// Step<Iterator> either(Token&,

template <typename Iterator>
Step<Iterator> start(Iterator begin, Iterator end) {
  return Step<Iterator>(begin, end);
}
}

#endif
