#ifndef SP_CPP_META_FUNCTION_BODY_PARSER_H
#define SP_CPP_META_FUNCTION_BODY_PARSER_H

#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class StackScopeParser : public match::Base<StackScopeAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = StackScopeAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // return StepType(start.it, start.end, false);
    return start;
  }
};

} // namspeace ast

#endif
