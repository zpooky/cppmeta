#ifndef SP_CPP_META_CLASS_FUNCTION_PARSER_H
#define SP_CPP_META_CLASS_FUNCTION_PARSER_H

#include "ParameterParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {
template <typename Iterator>
class FunctionDefinitionParser
    : public match::Base<FunctionDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(FunctionDefinitionAST &capture, StepType step) const {
    std::vector<Token> prefix;
    ParameterTypeAST returnType;
    Token functionName;
    std::vector<ParameterAST> parameters;
    Token constMemberFunction;
    auto ret = step //
                   .repeat(prefix, match::Either({"virtual", "static", "extern",
                                                "override"})) //
                   .step(returnType, ParameterTypeParser<Iterator>())
                   .step(functionName, FunctionName<Iterator>())
                   .step("(")                                            //
                   .repeat(parameters, ParameterParser<Iterator>(), ",") //
                   .step(")")                                            //
                   .option(constMemberFunction, "const")//
                   // .option([&](
    .step(";") //
        ;
    if (ret) {
      capture = FunctionDefinitionAST(returnType, functionName, parameters);
    }
    return ret;
  }
};
}

#endif
