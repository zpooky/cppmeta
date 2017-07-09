#ifndef SP_CPP_META_EXPRESSION_PARSER_H
#define SP_CPP_META_EXPRESSION_PARSER_H

#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class TypeExpressionParser : public match::Base<TypeExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(TypeExpressionAST &result, StepType start) const {
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
class ExpressionParser : public match::Base<ExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(ExpressionAST &result, StepType start) const {
    // TODO
    Token e;
    auto ret = start.step(e);
    if (ret) {
      result = ExpressionAST({e});
    }
    return ret;
  }
};
}

#endif
