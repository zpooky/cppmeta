#ifndef SP_CPP_META_CLASS_TEMPLATE_H
#define SP_CPP_META_CLASS_TEMPLATE_H

#include "ExpressionParser.h"
#include "matcher.h"
#include "ast.h"
#include "Pattern.h"

namespace ast {

template <typename Iterator>
class TypenameParser : public match::Base<std::vector<tmp::TypenameAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(std::vector<tmp::TypenameAST> &, StepType start) const {
    return match::either(start,
                         [&](StepType it) -> StepType { //
                           Token t;
                           Token type;
                           ExpressionAST exp;
                           return it                                          //
                               .step(t, match::Either({"class", "typename"})) //
                               .option(type, TypeName<Iterator>())            //
                               .option([&](StepType s) {
                                 return s
                                     .step("=") //
                                     .step(exp, ExpressionParser<Iterator>());
                               });

                         },
                         [&](StepType it) -> StepType { //
                           TypeIdentifier type;
                           Token name;
                           ExpressionAST exp;
                           return it                                         //
                               .step(type, TypeIdentifierParser<Iterator>()) //
                               .step(name, VariableName<Iterator>())         //
                               .option([&](StepType s) {
                                 return s
                                     .step("=") //
                                     .step(exp, ExpressionParser<Iterator>());
                               });
                         });
  }
}; // TypenamedParser

template <typename Iterator>
class TemplateParser : public match::Base<std::vector<tmp::TypenameAST>, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  TemplateParser() {
  }

  match::Step<Iterator> operator()(std::vector<tmp::TypenameAST> &result,
                                   match::Step<Iterator> step) const {
    return step                                          //
        .step("template")                                //
        .step("<")                                       //
        .repeat(result, TypenameParser<Iterator>(), ",") //
        .step(">");
  } // templated
};  // TemplateParser

}

#endif
