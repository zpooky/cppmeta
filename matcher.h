#ifndef SP_META_CPP_AST_ENTETIES_H
#define SP_META_CPP_AST_ENTETIES_H

#include "String.h"
#include "tokens.h"
#include <type_traits>
#include <utility>
#include <vector>

namespace match {

template <typename, typename>
class Base;

template <typename Iterator>
struct Step;

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

/*either()*/
template <typename Iterator>
Step<Iterator> either(Step<Iterator>);

template <typename Iterator, typename Function1, typename... Function>
Step<Iterator> either(Step<Iterator>, Function1, Function...);

/*class Step*/
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
  SelfType step(const char *constant) const {
    return step(String(constant));
  }

  SelfType step(const String &constant) const {
    if (valid) {
      if (it != end) {
        if (*it == constant) {
          return Step(it + 1, end, true);
        }
      }
    }
    return Step(it, end, false);
  }

  SelfType step(Token &capture) const {
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

  template <typename Function>
  // typename std::enable_if<!std::is_same<String, Function>::value,
  // SelfType>::type
  SelfType stepx(Function f) {
    if (valid) {
      if (it != end) {
        SelfType res = f(*this);
        if (res.valid) {
          // assert(res != *this);
          return res;
        }
      }
    }
    return SelfType(it, end, false);
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

  template <size_t N>
  SelfType step(Token &t, const char (*constant)[N]) {
    return step(t, String(constant, N));
  }
  SelfType step(Token &t, const char *constant) {
    return step(t, String(constant));
  }
  SelfType step(Token &t, const String &constant) {
    if (valid) {
      if (it != end) {
        if (*it == constant) {
          t = *it;
          return SelfType(it + 1, end, true);
        }
      }
    }
    return SelfType(it, end, false);
  }

  template <typename Out>
  SelfType step(Out &out, const Base<Out, Iterator> &f) {
    if (valid) {
      if (it != end) {
        auto result = f(out, *this);
        // assert(result.valid && result != *this);
        return result;
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

  // SelfType option(const Either &e) {
  //   if (valid) {
  //     if (it != end) {
  //       if (e.match(*it)) {
  //         return Step(it + 1, end, valid);
  //       }
  //     }
  //   }
  //   return Step(it, end, valid);
  // }

  template <typename Out>
  SelfType option(Out &out, const Base<Out, Iterator> &f) {
    if (valid) {
      if (it != end) {
        SelfType res = f(out, *this);
        if (res) {
          assert(res != *this);
          return res;
        }
      }
    }
    return SelfType(it, end, valid);
  }

  SelfType option(const char *constant) {
    return option(String(constant));
  }
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

  template <size_t N>
  SelfType option(Token &t, const char (*constant)[N]) {
    return option(t, String(constant, N));
  }
  SelfType option(Token &t, const char *constant) {
    return option(t, String(constant));
  }
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
        assert(current != *this);
        return current;
      }
    }
    return *this;
  }

  template <typename Out, typename Function>
  typename std::enable_if<!std::is_same<Function, String>::value,
                          SelfType>::type
      // SelfType
      repeat(Out &out, Function f) {
    SelfType current = *this;
  start:
    if (current.valid) {
      typename Out::value_type capture;
      SelfType result = f(capture, current);
      if (result.valid) {
        assert(result != current);
        out.push_back(capture);
        current = result;
        if (current.it != current.end) {
          goto start;
        }
      }
    }
    return current;
  }

  template <typename Out>
  SelfType repeat(Out &out, const match::Either &either) {
    SelfType current = *this;
  start:
    if (current.valid) {
      if (current.it != current.end) {
        if (either.match(*current.it)) {
          out.push_back(*current.it);
          current = SelfType(current.it + 1, end, true);
          goto start;
        }
      }
    }
    return current;
  }

  template <typename Function>
  SelfType repeat(Function f, const String &separator) {
    SelfType current = *this;
  start:
    if (current.valid) {
      SelfType result = f(current);
      if (result) {
        assert(result != current);
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
      typename Out::value_type capture;
      SelfType result = f(capture, current);
      if (result) {
        assert(result != current);
        out.push_back(capture);
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

  // template <typename... Function>
  // SelfType either(Function... f) {
  //   return this;
  // }

  operator Iterator() {
    return it;
  }

  operator bool() {
    return valid;
  }

  bool operator==(const SelfType &o) const {
    return o.it == it && valid == o.valid;
  }

  bool operator!=(const SelfType &o) const {
    return !operator==(o);
  }

  template <typename Function1, typename... Function>
  Step<Iterator> eitherx(Function1 first, Function... tail) {
    return either(*this, first, tail...);
  }
}; // class Step

template <typename Iterator>
Step<Iterator> start(Iterator begin, Iterator end) {
  return Step<Iterator>(begin, end);
}

template <typename Iterator>
Step<Iterator> either(Step<Iterator> s) {
  return Step<Iterator>(s.it, s.end, false);
}

template <typename Iterator, typename Function1, typename... Function>
Step<Iterator> either(Step<Iterator> s, Function1 first, Function... tail) {
  if (s.valid) {
    Step<Iterator> r = first(s);
    if (r) {
      return r;
    }
    return either(s, tail...);
  }
  return Step<Iterator>(s.it, s.end, false);
} // either()

template <typename T, typename Iterator>
class Base {
public:
  virtual Step<Iterator> operator()(T &, Step<Iterator> step) const = 0;
}; // class Base

/*join()*/
template <typename... T>
Token join(T... t) {
  // TODO
  return Token();
}

template <typename Collection>
typename std::enable_if<
    std::is_same<typename Collection::value_type, Token>::value, Token>::type
joins(const Collection &c) {
  return Token();
}

/*TokenJoinParser*/
template <typename Iterator>
class TokenJoinParser : public match::Base<Token, Iterator> {
private:
  using StepType = match::Step<Iterator>;

  std::vector<String> m_compare;

  StepType match(StepType start, const String &match,
                 std::vector<Token> &capture) const {
    StepType it = start;
    String tmp;
    while (match.starts_with(tmp) && it.it != it.end) {
      if (match == tmp) {
        return it;
      }
      tmp.append((*it.it).token);
      capture.push_back(*it.it);
      it = StepType(it.it + 1, it.end, it.valid);
    }

    return StepType(start.it, start.end, false);
  }

public:
  using capture_type = Token;

  TokenJoinParser(const std::vector<String> &match) //
      : m_compare(match) {
  }

  StepType operator()(capture_type &capture, StepType start) const {
    for (const auto &current : m_compare) {
      std::vector<Token> matching;
      auto ret = match(start, current, matching);
      if (ret.valid) {
        capture = join(matching);
        return ret;
      }
    }
    return StepType(start.it, start.end, false);
  }
};

} // namespace match

#endif
