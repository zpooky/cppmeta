#ifndef SP_CPP_META_FUNCTION_INVOCATION_PARSER_H
#define SP_CPP_META_FUNCTION_INVOCATION_PARSER_H

#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class FunctionInvocationParser
    : public match::Base<FunctionInvocationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(FunctionInvocationAST &result, StepType start) const {
    return start;
  }
};

}

#endif
