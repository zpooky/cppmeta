#ifndef SP_CPP_META_CLASS_PARSER_H
#define SP_CPP_META_CLASS_PARSER_H

#include "Pattern.h"
#include "TemplateParser.h"
#include "ast.h"
#include "matcher.h"

#include <stdio.h>

namespace ast {
template <typename Iterator>
class InheritanceParser
    : public match::Base<std::vector<InheritanceAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = std::vector<InheritanceAST>; 

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
class ClassParser : public match::Base<ClassAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ClassAST;

  StepType operator()(ClassAST &capture, StepType start) const {
    Token typeQualifier;
    // match::Either qualifier({"class", "struct"});
    Token name;

    std::vector<InheritanceAST> inherits;
    std::vector<tmp::TemplateTypenameAST> templates;

    return start
        .option(templates, TemplateParser<Iterator>())           //
        .step(typeQualifier, match::Either({"class", "struct"})) //
        .step(name, TypeName<Iterator>())                        //
        .option(inherits, InheritanceParser<Iterator>())         //
        .step("{")
        .stepx([&](StepType it) {
          match::Either scopes({"public", "private", "protected"});
          Token scope;
          auto scopeStart = it                       //
                                .step(scope, scopes) //
                                .step(":");
          // return scopeStart;
          capture = ClassAST(name, inherits, templates);
          return it;
        })         //
        .step("}") //
        .step(";");
    // printf("%d\n", start.valid);

    // ScopeParser scope;
    // ScopeAST scope_ast = scope.parse(it, end);
  }
}; // ClassParser
} // namespace ast

#endif
