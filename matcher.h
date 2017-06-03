#ifndef SP_META_CPP_AST_ENTETIES_H
#define SP_META_CPP_AST_ENTETIES_H

#include "String.h"
#include "tokens.h"
#include <type_traits>
#include <utility>
#include <vector>

namespace ast {}

namespace match {

template <typename, typename>
class Base;

struct Either {
  std::vector<String> constants;

  // template <typename... Tail>
  // Either(Tail &&... tail) : constants{std::forward<String>(tail)...} {
  // }

  Either(const std::initializer_list<String> &s) : constants(s) {
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

// template <typename... Tail>
// Either either(const Tail &... tail);
//
// template <typename... Tail>
// Either either(Tail... tail) {
//   return Either(std::forward<String>(tail)...);
// }

template <typename Iterator>
struct Step {
public:
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

  // SelfType step(Token &token, const Pattern &p) {
  //   if (valid) {
  //     if (it != end) {
  //       if (p.match(*it)) {
  //         token = *it;
  //         return Step(it + 1, end, true);
  //       }
  //     }
  //   }
  //   return Step(it, end, false);
  // }

  template <typename Out>
  SelfType step(Out &out, const Base<Out, Iterator> &f) {
    if (valid) {
      if (it != end) {
        SelfType s = *this;
        return f(out, s);
      }
    }
    return SelfType(it, end, valid);
  }

  SelfType option(Token &token, const Either &e) {
    if (valid) {
      if (it != end) {
        if (e.match(*it)) {
          token = *it;
          return Step(it + 1, end, valid);
        }
      }
    }
    return Step(it, end, valid);
  }

  template <typename Out>
  SelfType option(Out &out, const Base<Out, Iterator> &f) {
    if (valid) {
      if (it != end) {
        SelfType res = f(out, *this);
        if (res) {
          return res;
        }
      }
    }
    return SelfType(it, end, valid);
  }

  SelfType option(const char *) = delete;
  SelfType option(const String &constant) {
    if (valid) {
      if (it != end) {
        if (*it == constant) {
          return SelfType(it + 1, end, valid);
        }
      }
    }
    return SelfType(it, end, valid);
  }

  SelfType option(Token &, const char *) = delete;
  SelfType option(Token &t, const String &constant) {
    if (valid) {
      if (it != end) {
        if (*it == constant) {
          t = *it;
          return SelfType(it + 1, end, valid);
        }
      }
    }
    return SelfType(it, end, valid);
  }

  // private:
  //   SelfType either(Token &match) {
  //     return SelfType(it, end, false);
  //   }
  //
  // public:
  //   template <typename Constant, typename... Constants>
  //   SelfType either(Token &match, const Constant &head,
  //                   const Constants &... tail) {
  //     if (it != end && valid) {
  //       if (*it == head) {
  //         match = *it;
  //         return SelfType(it + 1, end, true);
  //       }
  //       return either(match, tail...);
  //     } else {
  //       return SelfType(it, end, false);
  //     }
  //   }
  //
  // template <typename... Term>
  // SelfType combination(Token &, const String &, const Term &...);
  //
  // template <typename... Term>
  // SelfType combination(Token &, const Either &, const Term &...);

  // private:
  //   SelfType combination(Token &match) {
  //     return SelfType(it, end, valid);
  //   }
  //
  // public:
  //   template <typename... Term>
  //   SelfType combination(std::vector<Token> &match, const String &c,
  //                        const Term &... terms) {
  //     return SelfType(it, end, valid);
  //   }
  //
  //   template <typename... Term>
  //   SelfType combination(std::vector<Token> &match, const Either &e,
  //                        const Term &... terms) {
  //     return SelfType(it, end, valid);
  //   }
  //
  template <typename Function>
  SelfType option(Function f) {
    if (valid) {
      Step<Iterator> current = f(*this);
      if (current.valid) {
        return current;
      }
    }
    return *this;
  }

  template <typename Function>
  SelfType repeat(Function f, const String &separator) {
    SelfType current = *this;
  start:
    if (current.valid) {
      SelfType result = f(current);
      if (result) {
        current = result;
        if (current.it != current.end) {
          if (*current.it == separator) {
            current = SelfType(current.it + 1, current.end, true);
            goto start;
          }
        }
      }
    }
    return current;
  }

  template <typename Out, typename Function>
  SelfType repeat(Out &out, Function f, const String &separator) {
    SelfType current = *this;
  start:
    if (current.valid) {
      SelfType result = f(out, current);
      if (result) {
        current = result;
        if (current.it != current.end) {
          if (*current.it == separator) {
            current = SelfType(current.it + 1, current.end, true);
            goto start;
          }
        }
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

  void replace(const std::vector<Token>&){
  }
}; // class Step

template <typename Iterator>
Step<Iterator> start(Iterator begin, Iterator end) {
  return Step<Iterator>(begin, end);
}

template <typename Iterator, typename Function, typename Function2>
Step<Iterator> either(Step<Iterator> s, Function first, Function2 second) {
  if (s.valid) {
    {
      Step<Iterator> r = first(s);
      if (r) {
        return r;
      }
    }
    {
      Step<Iterator> r = second(s);
      if (r) {
        return r;
      }
    }
  }
  return Step<Iterator>(s.it, s.end, false);
}

template <typename T, typename Iterator>
class Base {
public:
  virtual Step<Iterator> operator()(T &, Step<Iterator> step) const = 0;
}; // class Base

} // namespace match

#endif
