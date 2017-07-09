#ifndef SP_CPP_META_CLASS_TEMPLATE_H
#define SP_CPP_META_CLASS_TEMPLATE_H

#include "ExpressionParser.h"
#include "ParameterParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class TemplateTypenameParser
    : public match::Base<tmp::TemplateTypenameAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = tmp::TemplateTypenameAST;

  StepType operator()(capture_type &capture, StepType start) const {
    return match::either(
        start,
        [&](StepType it) -> StepType { //
          Token junk;
          Token type;
          stk::TypeExpressionAST exp;
          // ex: typename Type = Type
          // ex: typename Type
          auto ret = it                                                    //
                         .step(junk, match::Either({"class", "typename"})) //
                         .option(type, TypeName<Iterator>())               //
                         .option([&exp](StepType s) {
                           return s
                               .step("=") //
                               .step(exp, stk::TypeExpressionParser<Iterator>());
                         });
          if (ret) {
            // TODO
            // capture = tmp::TemplateTypenameAST(type, exp);
          }
          return ret;
        },
        [&](StepType it) -> StepType { //
          ParameterEither type;
          std::vector<Token> ref;
          Token name;
          stk::ExpressionAST exp;
          // ex: std::Type<wasd<asd>>& label
          // ex: std::uint32_t = 32
          auto ret = it                                                 //
                         .step(type, ParameterEitherParser<Iterator>()) //
                         .repeat(ref, match::Either({"&", "*"}))        //
                         .option(name, VariableName<Iterator>())        //
                         .option([&exp](StepType it) {
                           return it
                               .step("=") //
                               .step(exp, stk::ExpressionParser<Iterator>());
                         });
          ;
          if (ret) {
            // TODO
            // capture = tmp::TemplateTypenameAST(type, ref, name, exp);
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
  using capture_type = std::vector<tmp::TemplateTypenameAST>;

  StepType operator()(capture_type &result, StepType step) const {
    return step                                                  //
        .step("template")                                        //
        .step("<")                                               //
        .repeat(result, TemplateTypenameParser<Iterator>(), ",") //
        .step(">");
  }
}; // TemplateParser
}

#endif
