#ifndef SP_CPP_META_CLASS_PARSER_H
#define SP_CPP_META_CLASS_PARSER_H

#include "Pattern.h"
#include "ast.h"
#include "matcher.h"
#include <stdio.h>

namespace ast {
template <typename Iterator>
class InheritanceParser
    : public match::Base<std::vector<InheritanceAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(std::vector<InheritanceAST> &result,
                      StepType step) const {
    return step    //
        .step(":") //
        .repeat(
            [&](StepType start) { //
              const match::Either scopes({"public", "private", "protected"});
              const String virtuals = "virtual";
              return match::either(
                  start,
                  [&](StepType it) {
                    Token scope;
                    Token virt;
                    TypeIdentifier from;
                    StepType r =
                        it                          //
                            .option(scope, scopes)  //
                            .option(virt, virtuals) //
                            .step(from, TypeIdentifierParser<Iterator>());
                    if (r.valid) {
                      result.emplace_back(scope, virt, from);
                    }
                    return r;
                  },
                  [&](StepType it) {
                    Token scope;
                    Token virt;
                    TypeIdentifier from;
                    StepType r =
                        it                          //
                            .option(virt, virtuals) //
                            .option(scope, scopes)  //
                            .step(from, TypeIdentifierParser<Iterator>());

                    if (r.valid) {
                      result.emplace_back(scope, virt, from);
                    }
                    return r;
                  });
            },
            ",");
  }
}; // InheritanceParser

template <typename Iterator>
class TypenameParser
    : public match::Base<std::vector<TemplateParamterAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(std::vector<TemplateParamterAST> &,
                      StepType start) const {
    return match::either(start,
                         [&](StepType it) -> StepType { //
                           Token t;
                           Token type;
                           return it //
                               .step(t, match::Either({"class", "typename"}))
                               //
                               .step(type, TypeName<Iterator>());
                         },
                         [&](StepType it) -> StepType { //
                           TypeIdentifier type;
                           Token name;
                           return it                                         //
                               .step(type, TypeIdentifierParser<Iterator>()) //
                               .step(name, VariableName<Iterator>());
                         });
  }
}; // TypenamedParser

template <typename Iterator>
class TemplateParser
    : public match::Base<std::vector<TemplateParamterAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  TemplateParser() {
  }

  match::Step<Iterator> operator()(std::vector<TemplateParamterAST> &result,
                                   match::Step<Iterator> step) const {
    return step                                          //
        .step("template")                                //
        .step("<")                                       //
        .repeat(result, TypenameParser<Iterator>(), ",") //
        .step(">");
  } // templated
};  // TemplateParser

template <typename Iterator>
class ClassParser {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType parse(Iterator begin, Iterator end, ClassAST &result) {
    Token typeQualifier;
    // match::Either qualifier({"class", "struct"});
    Token name;

    std::vector<InheritanceAST> inherits;
    std::vector<TemplateParamterAST> templates;

    match::Step<Iterator> start =
        match::start(begin, end)                                     //
            .option(templates, TemplateParser<Iterator>())           //
            .step(typeQualifier, match::Either({"class", "struct"})) //
            .step(name, TypeName<Iterator>())                        //
            .option(inherits, InheritanceParser<Iterator>())         //
            .step("{")                                               //
        ;
    printf("%d\n", start.valid);

    result = ClassAST(name, inherits, templates);
    if (start) {
      Token scope;
      match::Either scopes_match({"public", "private", "protected"});
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
    return start.step("}").step(";");
  }
}; // ClassParser
} // namespace ast

#endif
