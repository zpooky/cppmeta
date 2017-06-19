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
  StepType operator()(ClassAST &result, StepType start) const {
    Token typeQualifier;
    // match::Either qualifier({"class", "struct"});
    Token name;

    std::vector<InheritanceAST> inherits;
    std::vector<tmp::TemplateTypenameAST> templates;

    match::Step<Iterator> begin =
        start
            .option(templates, TemplateParser<Iterator>())           //
            .step(typeQualifier, match::Either({"class", "struct"})) //
            .step(name, TypeName<Iterator>())                        //
            .option(inherits, InheritanceParser<Iterator>())         //
            .step("{");
    // printf("%d\n", start.valid);

    result = ClassAST(name, inherits, templates);
    if (begin) {
      Token scope;
      match::Either scopes_match({"public", "private", "protected"});
      auto scopeStart = begin                          //
                            .step(scope, scopes_match) //
                            .step(":");
      if (!scopeStart) {
        // scope = Token("private");
      }

      // ScopeParser scope;
      // ScopeAST scope_ast = scope.parse(it, end);
      // class_ast.push_back(scope, scope_ast);
    }
    return begin.step("}").step(";");
  }
}; // ClassParser
} // namespace ast

#endif
