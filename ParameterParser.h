#ifndef SP_CPP_META_PARAMETER_PARSER_H
#define SP_CPP_META_PARAMETER_PARSER_H

#include "ExpressionParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {

template <typename Iterator>
class ParameterTypeParser : public match::Base<ParameterTypeAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(ParameterTypeAST &capture, StepType start) const {
    std::vector<Token> qualifiers;
    TypeIdentifier type;
    std::vector<Token> refs;
    std::vector<Token> ptrs;
    auto ret = start                                                         //
                   .repeat(qualifiers, match::Either({"const", "volatile"})) //
                   .step(type, TypeIdentifierParser<Iterator>())             //
                   .repeat(refs, match::Either({"&"}))                       //
                   .repeat(ptrs, match::Either({"*"}));
    if (ret) {
      capture = ParameterTypeAST(qualifiers, type, refs, ptrs);
    }
    return ret;
  }
};

template <typename Iterator>
class ParameterParser : public match::Base<ParameterAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(ParameterAST &capture, StepType start) const {
    ParameterTypeAST type;
    Token name;
    ExpressionAST v;
    auto ret = start                                            //
                   .step(type, ParameterTypeParser<Iterator>()) //
                   .option(name, VariableName<Iterator>())      //
                   .option([&](StepType it) {                   //
                     return it                                  //
                         .step("=")                             //
                         .step(v, ExpressionParser<Iterator>());
                   }); //
    if (ret) {
      capture = ParameterAST(type, name, v);
    }
    return ret;
  }
};

template <typename Iterator>
class FunctionPointerParser : public match::Base<FunctionPointerAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(FunctionPointerAST &capture, StepType start) const {
    ParameterTypeAST returnType;
    Token ref;
    Token name;
    std::vector<ParameterAST> paramters;

    // ex: const Type<tt>* (*fp)(...)
    return start                                           //
        .step(returnType, ParameterTypeParser<Iterator>()) //
        .step("(")                                         //
        // variable capture
        .step(ref, match::Either({"*", "&"})) //
        .step(name, TypeName<Iterator>())     //
        .step(")")                            //
        .step("(")                            //
        // parameters
        .repeat(paramters, ParameterParser<Iterator>(), ",") //
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
