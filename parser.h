#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ast.h"
#include "entities.h"
#include "matcher.h"
#include <vector>

namespace match {
Pattern TypeName();
}

namespace ast {

class ClassParser {
public:
  template <typename Iterator>
  match::Step<Iterator> inheritance(match::Step<Iterator> &step,
                                    std::vector<InheritanceAST> &result) {
    auto it = step.it;

    return match::start(it, step.end) //
        .step(":")                    //
        .repeat(
            [&](match::Step<Iterator> start) { //
              auto scopes = match::Either("public", "private", "protected");
              auto virtuals = "virtual";
              return match::either(
                  start,
                  [&](match::Step<Iterator> it) {
                    Token scope;
                    Token virt;
                    Token from;
                    match::Step<Iterator> r =
                        it.option(scope, scopes)    //
                            .option(virt, virtuals) //
                            .step(from, match::TypeName());

                    if (r.valid) {
                      result.emplace_back(scope, virt, TypeName(from));
                    }
                    return r;
                  },
                  [&](match::Step<Iterator> it) {
                    Token scope;
                    Token virt;
                    Token from;
                    match::Step<Iterator> r =
                        it.option(virt, virtuals)  //
                            .option(scope, scopes) //
                            .step(from, match::TypeName());

                    if (r.valid) {
                      result.emplace_back(scope, virt, TypeName(from));
                    }
                    return r;
                  });
            },
            ",");
  }

  template <typename Iterator>
  ClassAST parse(Iterator begin, Iterator end) {
    Token name;
    std::vector<InheritanceAST> inherits;
    auto start =
        match::start(begin, end)                                             //
            .step("class")                                                   //
            .step(name, match::TypeName())                                   //
            .option([&](match::Step<Iterator> &o) -> match::Step<Iterator> { //
              return inheritance(o, inherits);
            }) //
            .step("{");

    Iterator it = start.it;

    ClassAST result(name, inherits);
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
    return result;
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

class Parser {
public:
  Parser() {
  }

  FileAST parse(const std::vector<Token> &);
};
}

#endif
