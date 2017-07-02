#ifndef SP_CPP_META_CLASS_PARSER_H
#define SP_CPP_META_CLASS_PARSER_H

#include "Pattern.h"
#include "ScopeParser.h"
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
                  [&result, &scopes, &virtuals](StepType it) {
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
                  [&result, &virtuals, &scopes](StepType it) {
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
    Token tq;
    Token name;

    std::vector<InheritanceAST> inherits;
    std::vector<tmp::TemplateTypenameAST> templates;

    //TODO capture templateSpecialization
    std::vector<TypeIdentifier> templateSpec;

    return start
        .option(templates, TemplateParser<Iterator>())         //
        .step(tq, match::Either({"union", "class", "struct"})) //
        .step(name, TypeName<Iterator>())                      //
        .option(templateSpec, TypeArgumentParser<Iterator>()) //
        .option(inherits, InheritanceParser<Iterator>())      //
        .step("{")
        .stepx([&name, &inherits, &templates, &tq, &capture](StepType it) {
          match::Either scopes({"public", "private", "protected"});

          ClassAST tmp(name, inherits, templates);
          ScopeAST *curScope = tq == "class" ? &tmp.privateAST : &tmp.publicAST;

        start:
          it = generic_scope(*curScope, it);
          if (!it) {
            return it;
          }

          {
            Token scopeToken;
            auto scopeStart = it                            //
                                  .step(scopeToken, scopes) //
                                  .step(":");
            if (scopeStart) {
              if (scopeToken == "public") {
                curScope = &tmp.publicAST;
              } else if (scopeToken == "private") {
                curScope = &tmp.privateAST;
              } else if (scopeToken == "protected") {
                curScope = &tmp.protectedAST;
              }
              it = scopeStart;
              goto start;
            }
          }
          capture = tmp;

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
