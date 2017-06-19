#ifndef SP_CPP_META_CLASS_TEMPLATE_H
#define SP_CPP_META_CLASS_TEMPLATE_H

#include "ExpressionParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class TemplateTypenameParser
    : public match::Base<tmp::TemplateTypenameAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(tmp::TemplateTypenameAST &capture, StepType start) const {
    return match::either(
        start,
        [&](StepType it) -> StepType { //
          Token junk;
          Token type;
          TypeExpressionAST exp;
          // ex: typename Type = Type
          // ex: typename Type
          auto ret = it                                                    //
                         .step(junk, match::Either({"class", "typename"})) //
                         .option(type, TypeName<Iterator>())               //
                         .option([&](StepType s) {
                           return s
                               .step("=") //
                               .step(exp, TypeExpressionParser<Iterator>());
                         });
          if (ret) {
            capture = tmp::TemplateTypenameAST(type);
          }
          return ret;
        },
        [&](StepType it) -> StepType { //
          TypeIdentifier type;
          Token name;
          ExpressionAST exp;
          Token ref;
          // ex: std::Type<wasd<asd>>& label
          // ex: int i = 1
          auto ret = it                                                //
                         .step(type, TypeIdentifierParser<Iterator>()) //
                         // .step(ref, match::Either({"&", "*", "**"}))   //
                         .step(name, VariableName<Iterator>()) //
                         .option([&](StepType s) {
                           return s
                               .step("=") //
                               .step(exp, ExpressionParser<Iterator>());
                         });
          if (ret) {
            // capture = tmp::TemplateTypenameAST(type, name);
          }
          return ret;
        },
        [&](StepType it) -> StepType {
          // ex: int wasd = 1
          Token type;
          Token variable;
          auto ret = it                                              //
                         .step(type)                                //
                         .option(variable, VariableName<Iterator>()) //
                         .option([&](StepType s) {
                           ExpressionAST exp;
                           return s
                               .step("=") //
                               .step(exp, ExpressionParser<Iterator>());
                         });
          if (ret) {
            // capture = tmp::TemplateTypenameAST(type, name);
          }
          return ret;
        });
  }
}; // TypenamedParser

template <typename Iterator>
class TemplateParser
    : public match::Base<std::vector<tmp::TemplateTypenameAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  TemplateParser() {
  }

  match::Step<Iterator>
  operator()(std::vector<tmp::TemplateTypenameAST> &result,
             match::Step<Iterator> step) const {
    return step                                                  //
        .step("template")                                        //
        .step("<")                                               //
        .repeat(result, TemplateTypenameParser<Iterator>(), ",") //
        .step(">");
  } // templated
};  // TemplateParser
}

#endif
