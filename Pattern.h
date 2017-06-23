#ifndef SP_CPP_META_CLASS_PATTERN_H
#define SP_CPP_META_CLASS_PATTERN_H

#include "ast.h"
#include "lang.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
struct Pattern : public match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

  StepType operator()(Token &t, StepType it) const {
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

    if (it.valid) {
      if (it.it != it.end) {
        t = *it.it;
      }
    }
    // printf("class: %s\n",t.token.c_str());

    auto next = it.valid && it.it != it.end ? it.it + 1 : it.it;
    return StepType(next, it.end, true);
  }
};

template <typename Iterator>
struct TypeName : public match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

  StepType operator()(Token &t, StepType it) const {
    if (it.valid) {
      if (it.it != it.end) {
        for (const char *keyword : cpp::keywords) {
          if (*it.it == keyword && *it.it != "void") {
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

    if (it.valid) {
      if (it.it != it.end) {
        t = *it.it;
      }
    }
    // printf("class: %s\n",t.token.c_str());

    auto next = it.valid && it.it != it.end ? it.it + 1 : it.it;
    return StepType(next, it.end, true);
  }
};

template <typename Iterator>
Pattern<Iterator> VariableName() {
  return {};
}

template <typename Iterator>
Pattern<Iterator> NamespaceName() {
  return {};
}

template <typename Iterator>
Pattern<Iterator> FunctionName() {
  return {};
}

// ex: ns::
template <typename Iterator>
struct NsParser : public match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

  StepType operator()(Token &ns, StepType it) const {
    return it     //
        .step(ns) //
        .step("::");
  }
};

template <typename Iterator>
struct TypeIdentifierParser;

template <typename Iterator>
struct TypeArgumentParser
    : public match::Base<std::vector<TypeIdentifier>, Iterator> {

  using StepType = match::Step<Iterator>;

  StepType operator()(std::vector<TypeIdentifier> &capture, StepType it) const {
    return it                                                   //
        .step("<")                                              //
        .repeat(capture, TypeIdentifierParser<Iterator>(), ",") //
        .step(">");
  }
};

// ex: ns::ns::Type<T,int>
template <typename Iterator>
struct TypeIdentifierParser : public match::Base<TypeIdentifier, Iterator> {

  using StepType = match::Step<Iterator>;

  StepType operator()(TypeIdentifier &result, StepType it) const {
    std::vector<Token> namespaces;
    Token t;
    std::vector<TypeIdentifier> typeArguments;
    auto next = it.repeat(namespaces, NsParser<Iterator>()) //
                    .step(t, TypeName<Iterator>())          //
                    .option(typeArguments, TypeArgumentParser<Iterator>());
    if (next) {
      result = TypeIdentifier(t, typeArguments, namespaces);
    }
    return next;
  }
};
}

#endif
