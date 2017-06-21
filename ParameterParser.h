#ifndef SP_CPP_META_PARAMETER_PARSER_H
#define SP_CPP_META_PARAMETER_PARSER_H

#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class FunctionPointerParser : public match::Base<FunctionPointerAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(FunctionPointerAST &capture, StepType start) const {
    TypeIdentifier returnType;
    Token ref;
    Token name;
    std::vector<Token> paramters;

    return start                                            //
        .step(returnType, TypeIdentifierParser<Iterator>()) //
        .step("(")                                          //
        // variable capture
        .step(ref, match::Either({"*", "&"})) //
        .step(name, TypeName<Iterator>())     //
        .step(")")                            //
        .step("(")                            //
        // parameters
        .repeat(paramters,
                [](ParameterAST &capture, StepType it) { //
                  TypeIdentifier type;
                  Token name;
                  // TODO make more advanced
                  return it                                         //
                      .step(type, TypeIdentifierParser<Iterator>()) //
                      .step(name, VariableName<Iterator>());
                },
                ",") //
        .step(")");
    ;
  }
};

template <typename Iterator>
class TemplateCArrayParser : public match::Base<TemplateCArrayAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(TemplateCArrayAST &capture, StepType start) const {
    return start;
  }
};
} // namespace ast

#endif
