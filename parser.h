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

template <typename Iterator>
struct TypeIdentifierParser {
  match::Step<Iterator> operator()(TypeIdentifier &, match::Step<Iterator> it) {
    return it;
  }
};

class ClassParser {
public:
  // template <typename Iterator>
  // auto typeIdentifier() {
  //   return [](TypeIdentifier &,
  //             match::Step<Iterator> start) -> match::Step<Iterator> {
  //     return start;
  //   };
  // }

  template <typename Iterator>
  match::Step<Iterator> inheritance(match::Step<Iterator> step,
                                    std::vector<InheritanceAST> &result) {
    return step    //
        .step(":") //
        .repeat(
            [&](match::Step<Iterator> start) { //
              auto scopes = match::Either("public", "private", "protected");
              auto virtuals = "virtual";
              return match::either(
                  start,
                  [&](match::Step<Iterator> it) {
                    Token scope;
                    Token virt;
                    TypeIdentifier from;
                    match::Step<Iterator> r =
                        it                          //
                            .option(scope, scopes)  //
                            .option(virt, virtuals) //
                            .step(from, TypeIdentifierParser<Iterator>());
                    if (r.valid) {
                      result.emplace_back(scope, virt, from);
                    }
                    return r;
                  },
                  [&](match::Step<Iterator> it) {
                    Token scope;
                    Token virt;
                    TypeIdentifier from;
                    match::Step<Iterator> r =
                        it                          //
                            .option(virt, virtuals) //
                            .option(scope, scopes)  //
                            .step(from,TypeIdentifierParser<Iterator>());

                    if (r.valid) {
                      result.emplace_back(scope, virt, from);
                    }
                    return r;
                  });
            },
            ",");
  } // inheritance

  template <typename Iterator>
  match::Step<Iterator> typenamed(match::Step<Iterator> start,
                                  std::vector<TemplateParamterAST> &templates) {
    return match::either(
        start,
        [&](match::Step<Iterator> it) -> match::Step<Iterator> { //
          Token t;
          Token type;
          return it                                        //
              .step(t, match::Either("class", "typename")) //
              .step(type, match::TypeName());
        },
        [&](match::Step<Iterator> it) -> match::Step<Iterator> { //
          Token type;
          Token name;
          return it                          //
              .step(type, match::TypeName()) //
              .step(name, match::TypeName());
        });
  } // typenamed

  template <typename Iterator>
  match::Step<Iterator> templated(match::Step<Iterator> step,
                                  std::vector<TemplateParamterAST> &result) {
    return step           //
        .step("template") //
        .step("<")        //
        .repeat(
            [&](match::Step<Iterator> start) -> match::Step<Iterator> { //
              return typenamed(start, result);
            },
            ",") //
        .step(">");
  } // templated

  template <typename Iterator>
  ClassAST parse(Iterator begin, Iterator end) {
    Token typeQualifier;
    Token name;
    std::vector<InheritanceAST> inherits;
    std::vector<TemplateParamterAST> templates;
    auto start =
        match::start(begin, end)                                             //
            .option([&](match::Step<Iterator> &o) -> match::Step<Iterator> { //
              return templated(o, templates);
            })
            .step(typeQualifier, match::Either("class", "struct"))           //
            .step(name, match::TypeName())                                   //
            .option([&](match::Step<Iterator> &o) -> match::Step<Iterator> { //
              return inheritance(o, inherits);
            }) //
            .step("{");

    ClassAST result(name, inherits, templates);
    if (start) {
      Token scope;
      match::Either scopes_match("public", "private", "protected");
      auto scopeStart = start                          //
                            .step(scope, scopes_match) //
                            .step(":");
      if (!scopeStart) {
        // scope = Token("private");
      }

      // ScopeParser scope;
      // ScopeAST scope_ast = scope.parse(it, end);
      // class_ast.push_back(scope, scope_ast);
    }
    start.step("}").step(";");
    return result;
  }
}; // class Parser

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
