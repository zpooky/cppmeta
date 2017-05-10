#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ast.h"
#include "astenteties.h"
#include "entities.h"
#include <vector>

class B {};
class S : public virtual B {};
class A : virtual public B {};

namespace {
class ClassParser {
public:
  template <typename Iterator>
  match::Step<Iterator> inheritance(match::Step<Iterator> &step,
                                    std::vector<InheritanceAST> &inherits) {
    auto it = step.it;

    std::vector<Token> incap;
    auto scope = match::Either("public", "private", "protected");
    auto in_step = step.combination(incap, scope, String("virtual"));

    if (!in_step) {
      incap.emplace_back("public", 0, 0); // TODO fix support for infered Token
      it = in_step.it;
    }

    Token type;
    match::Step<Iterator> result = match::start(it, step.end).step(type);
    if (result) {
      inherits.emplace_back(/*TODO*/ Incapsulation::PUBLIC, TypeName(type));
    }

    return result;
  }

  template <typename Iterator>
  ClassAST parse(Iterator begin, Iterator end) {
    Token name;
    std::vector<InheritanceAST> inherits;
    auto start = match::start(begin, end) //
                     .step("class")       //
                     .step(name)          //
                     .option(":")
                     .repeat(
                         [&](match::Step<Iterator> &step) { //
                           return inheritance(step, inherits);
                         },
                         ",") //
                     .step("{");
    Iterator it = start.it;

    ClassAST class_ast(name, inherits);
    if (start) {
      Token scope;
      auto scopeStart = match::start(it, end).step(scope).step(":");
      // it = scopeStart.it;
      if (!scopeStart) {
        // scope = Token("private");
      }

      // ScopeParser scope;
      // ScopeAST scope_ast = scope.parse(it, end);
      // class_ast.push_back(scope, scope_ast);
    }
    it = match::start(it, end).step("}").step(";");
    /*
     * typedef class {
     * } name;
     * class name {
     * };
     */
    // ClassAST ast;
    return class_ast;
  }
};

template <typename It>
bool is_tokens(It, It);

template <typename It, typename Head, typename... Tails>
bool is_tokens(It, It, const Head &, const Tails &...);

template <typename It>
bool is_tokens(It, It) {
  return true;
}

template <typename It, typename Head, typename... Tails>
bool is_tokens(It begin, It end, const Head &token, const Tails &... tail) {
  bool valid(false);
  if (begin != end) {
    valid = *begin == token;
  }
  return valid && is_tokens(begin + 1, end, tail...);
}

template <typename Iterator>
bool is_class(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "class");
}

template <typename Iterator>
bool is_include(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "#", "include");
}

template <typename Iterator>
bool is_define(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "#", "define");
}

template <typename Iterator>
bool is_struct(Iterator begin, Iterator end) {
  return is_tokens(begin, end, "typedef", "struct") ||
         is_tokens(begin, end, "struct");
}

template <typename Iterator>
bool is_function_declaration(Iterator it, Iterator end) {
  return false;
}
} // namesapce anon

class Parser {
public:
  Parser() {
  }

  FileAST parse(const std::vector<Token> &);
};

#endif
