#ifndef SP_CPP_META_EXPRESSION_PARSER_H
#define SP_CPP_META_EXPRESSION_PARSER_H

#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace stk {

template <typename Iterator>
struct TypeExpressionParser : match::Base<TypeExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = TypeExpressionAST;

  StepType operator()(capture_type &result, StepType start) const {
    // TODO
    Token e;
    auto ret = start.step(e);
    if (ret) {
      result = TypeExpressionAST(e);
    }
    return ret;
  }
};

template <typename Iterator>
struct NumericConstantParser : match::Base<NumericConstantAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = NumericConstantAST;

  StepType operator()(capture_type &result, StepType start) const {
    int i = +111, dd = -22, dkkd = 33, ffkf = 13ul, jfjf = +-11;
    Token sign;
    Token nom;
    Token fp;
    Token denom;
    return start                                            //
        .option(sign, match::Either({"-", "+"}))            //
        .step(nom, ast::NumericPattern<Iterator>())         //
        .optionx([&fp, &denom](StepType it) {               //
          return it                                         //
              .step(fp, ".")                                //
              .step(denom, ast::NumericPattern<Iterator>()) //
              ;
        });
  }
};

template <typename Iterator>
struct ExpressionParser : match::Base<ExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ExpressionAST;

  StepType operator()(capture_type &result, StepType start) const {
    // TODO
    Token e;
    auto ret = start.step(e, ast::VariableName<Iterator>());
    if (ret) {
      result = ExpressionAST({e});
    }
    return ret;
  }
};
}

#endif
