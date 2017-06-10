#ifndef SP_CPP_META_PREPROCESSOR_H
#define SP_CPP_META_PREPROCESSOR_H

#include "ArrayList.h"
#include "ast.h"
#include "matcher.h"
#include <iostream>
//
#define AS adaads \ asdasda

namespace ast {

template <typename Iterator>
class IfNotDefineMacroParser
    : public match::Base<IfNotDefinMacroAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  // IfNotDefineMacroParser(std::stack<MacroCondition>
  StepType operator()(IfNotDefinMacroAST &capture, StepType start) const {
    Token key;
    auto inital = start.step("#").step("ifndef").step(key);
    if (inital) {
      capture = IfNotDefinMacroAST(key);
    }
    return inital;
  }
};

template <typename Iterator>
class EndIfMacroParser : public match::Base<Token, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(Token &, StepType start) const {
    return start.step("#").step("endif");
  }
};

template <typename Iterator>
class DefineConstantParser : public match::Base<DefineAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(DefineAST &capture, StepType start) const {
    Token key;
    std::vector<Token> values;
    auto inital = start.step("#").step("define").step(key);
    if (inital) {
      auto line = key.location.line;
      auto current = inital;
      bool next_line = false;
      while (current.it != current.end &&
             ((*current.it).location.line == line || next_line)) {
        next_line = false;
        line = (*current.it).location.line;

        if ((*current.it).token == "\\") {
          next_line = true;
        } else {
          values.push_back(*current.it);
        }
        current = StepType(current.it + 1, current.end, true);
      }
      capture = DefineAST(key, values);
      return current;
    }
    return inital;
  }
};

template <typename Iterator>
class DefineSubstituteParser : public match::Base<Token, Iterator> {
private:
  using StepType = match::Step<Iterator>;
  const FileAST &file;

public:
  DefineSubstituteParser(const FileAST &p_file) : file(p_file) {
  }

  StepType operator()(Token &, StepType start) const {
    auto begin = file.defines.begin();
    auto end = file.defines.end();
    for (auto it = begin; it != end; ++it) {
      // printf("%s == %s\n", current.key.token.c_str(),
      // (*start.it).token.c_str());
      auto &current = *it;
      if (current.key == *start.it) {
        // (*start.it).token = current.values[0].token;
        start.it.replace(current.values);
        // TODO make better
        // start.replace(current.values);
        break;
      }
    }
    return StepType(start.it, start.end, false);
  }
};

class Preprocessor {
private:
  using Tokens = sp::ArrayList<Token>;
  using Iterator = Tokens::iterator;
  using StepType = match::Step<Iterator>;

  template <typename...>
  static StepType match(StepType);

  template <typename... Tail>
  static StepType match(StepType, String, Tail...);

  template <typename... Tail>
  static StepType match(StepType, Token &, Tail...);

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
    for_each(res, start, [&result](StepType current) -> StepType {
      {
        Token t;
        auto ret = match::either(
            current,
            [&t](StepType it) -> StepType {
              //
              // return match(it, "#", "include", "<", t, ">");
              return it.step("#").step("include").step("<").step(t).step(">");
            },
            [&t](StepType it) -> StepType {
              //
              // return
              // it.step("#").step("include").step("\"").step(t).step("\"");
              return match(it, "#", "include", "\"", t, "\"");
            });
        // printf("#####################%s\n", t.token.c_str());
        if (ret) {
          result.push_back(IncludeAST(t));
          return ret;
        }
      }
      {
        DefineAST ast;
        auto ret = current.step(ast, DefineConstantParser<Iterator>());
        if (ret) {
          result.push_back(ast);
          return ret;
        }
      }
      {
        IfNotDefinMacroAST ast;
        auto ret = current.step(ast, IfNotDefineMacroParser<Iterator>());
        if (ret) {
          result.push_back(ast);
          return ret;
        }
      }
      {
        Token dummy;
        auto ret = current.step(dummy, EndIfMacroParser<Iterator>());
        if (ret) {
          return ret;
        }
      }
      Token dummy;
      return current.step(dummy, DefineSubstituteParser<Iterator>(result));
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
