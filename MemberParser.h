#ifndef SP_CPP_META_MEMBER_PARSER_H
#define SP_CPP_META_MEMBER_PARSER_H

#include "ParameterParser.h"
#include "ast.h"
#include "matcher.h"

namespace ast {
/*FunctionDefinitionParser*/
template <typename Iterator>
class MemberDefinitionParser
    : public match::Base<MemberDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = MemberDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    ParameterEither type;
    Token name;
    auto ret = start                                              //
                   .step(type, ParameterEitherParser<Iterator>()) //
                   // option because of fp or tcarr
                   .option(name, VariableName<Iterator>()) //
                   .step(";");
    if (ret) {
      capture = MemberDefinitionAST(type, name);
    }
    return ret;
  }
};

} // namespace ast

#endif
