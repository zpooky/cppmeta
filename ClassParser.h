#ifndef SP_CPP_META_CLASS_PARSER_H
#define SP_CPP_META_CLASS_PARSER_H

#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {
template <typename Iterator>
class ClassParser {
  using StepType = match::Step<Iterator>;
  // using SelfType = ClassParser<Iterator>;

  StepType inheritance(StepType step, std::vector<InheritanceAST> &result) {
    return step    //
        .step(":") //
        .repeat(
            [&](StepType start) { //
              auto scopes = match::Either("public", "private", "protected");
              auto virtuals = "virtual";
              return match::either(
                  start,
                  [&](StepType it) {
                    Token scope;
                    Token virt;
                    TypeIdentifier from;
                    StepType r = it                          //
                                     .option(scope, scopes)  //
                                     .option(virt, virtuals) //
                                     .step(from, TypeIdentifierParser());
                    if (r.valid) {
                      result.emplace_back(scope, virt, from);
                    }
                    return r;
                  },
                  [&](StepType it) {
                    Token scope;
                    Token virt;
                    TypeIdentifier from;
                    StepType r = it                          //
                                     .option(virt, virtuals) //
                                     .option(scope, scopes)  //
                                     .step(from, TypeIdentifierParser());

                    if (r.valid) {
                      result.emplace_back(scope, virt, from);
                    }
                    return r;
                  });
            },
            ",");
  } // inheritance

  StepType typenamed(StepType start,
                     std::vector<TemplateParamterAST> &templates) {
    return match::either(start,
                         [&](StepType it) -> StepType { //
                           Token t;
                           Token type;
                           return it                                        //
                               .step(t, match::Either("class", "typename")) //
                               .step(type, TypeName());
                         },
                         [&](StepType it) -> StepType { //
                           TypeIdentifier type;
                           Token name;
                           return it                               //
                               .step(type, TypeIdentifierParser()) //
                               .step(name, VariableName());
                         });
  } // typenamed

  StepType templated(StepType step, std::vector<TemplateParamterAST> &result) {
    return step           //
        .step("template") //
        .step("<")        //
        .repeat(
            [&](StepType start) -> StepType { //
              return typenamed(start, result);
            },
            ",") //
        .step(">");
  } // templated

public:
  StepType parse(Iterator begin, Iterator end, ClassAST &result) {
    Token typeQualifier;
    Token name;
    std::vector<InheritanceAST> inherits;
    std::vector<TemplateParamterAST> templates;
    auto start = match::start(begin, end)                   //
                     .option([&](StepType &o) -> StepType { //
                       return templated(o, templates);
                     })
                     .step(typeQualifier, match::Either("class", "struct")) //
                     .step(name, TypeName())                                //
                     .option([&](StepType &o) -> StepType {                 //
                       return inheritance(o, inherits);
                     }) //
                     .step("{");

    result = ClassAST(name, inherits, templates);
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
    return start.step("}").step(";");
  }
}; // class Parser
} // namespace ast

#endif
