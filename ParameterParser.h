#ifndef SP_CPP_META_PARAMETER_PARSER_H
#define SP_CPP_META_PARAMETER_PARSER_H

#include "ExpressionParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace ast {
/*ParameterTypeParser*/
template <typename>
class ParameterTypeParser;

/*ParameterParser*/
template <typename>
class ParameterParser;

/*FunctionPointerParser*/
template <typename>
class FunctionPointerParser;

/*TemplateCArrayParser*/
template <typename>
class TemplateCArrayParser;

template <typename Iterator>
class ParameterEitherParser;
} // namespace ast

namespace ast {

template <typename Iterator>
class ParameterTypeParser : public match::Base<ParameterTypeAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(ParameterTypeAST &capture, StepType start) const {
    std::vector<Token> typeQualifiers;
    TypeIdentifier type;
    std::vector<Token> refs;
    std::vector<Token> ptrs;
    Token constPointer; // TODO
    // const byte* const it
    // TODO union { paramter / carray / fp / template array }
    // int (&pa)[5]
    auto ret =
        start                                                             //
            .repeat(typeQualifiers, match::Either({"const", "volatile"})) //
            .step(type, TypeIdentifierParser<Iterator>())                 //
            // TODO []
            .repeat(ptrs, match::Either({"*"})) //
            .repeat(refs, match::Either({"&"})) //
            .option(constPointer, "const")      //
        ;                                       //
    ;
    if (ret) {
      capture = ParameterTypeAST(typeQualifiers, type, refs, ptrs);
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
    ParameterEither type;
    Token name;
    ExpressionAST v;
    auto ret = start //
                   .step(type, ParameterEitherParser<Iterator>())
                   .option(name, VariableName<Iterator>()) //
                   .option([&](StepType it) {              //
                     return it                             //
                         .step("=")                        //
                         .step(v, ExpressionParser<Iterator>());
                   }); //
    if (ret) {
      capture = ParameterAST(std::move(type), name, v);
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

    // TODO member fp:
    // double (SomeClass::*function)(double *x) const;
    //#
    // ex: const Type<tt>* (*fp)(...)
    auto ret = start                                                  //
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
    if (ret) {
      capture = FunctionPointerAST(returnType, ref, name, paramters);
    }
    return ret;
  }
};

template <typename Iterator>
class TemplateCArrayParser : public match::Base<TemplateCArrayAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(TemplateCArrayAST &capture, StepType start) const {
    ParameterTypeAST returnType;
    std::vector<Token> ptr;
    Token variable;
    Token size;
    return start                                           //
        .step(returnType, ParameterTypeParser<Iterator>()) //
        .step("(")                                         //
        .repeat(ptr, match::Either({"*", "&"}))            //
        .option(variable, VariableName<Iterator>())        //
        .step(")")                                         //
        .step("[")                                         //
        // TODO expression
        .step(size) //
        .step("]");
  }
};

/*ParameterEitherParser*/
template <typename Iterator>
class ParameterEitherParser : public match::Base<ParameterEither, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ParameterEither;
  StepType operator()(capture_type &capture, StepType start) const {
    return start //
        .eitherx(
            [&capture](StepType it) {
              FunctionPointerAST type;
              auto ret = it.step(type, FunctionPointerParser<Iterator>());
              if (ret) {
                capture = std::move(type);
              }
              return ret;
            },
            [&capture](StepType it) {
              TemplateCArrayAST type;
              auto ret = it.step(type, TemplateCArrayParser<Iterator>());
              if (ret) {
                capture = std::move(type);
              }
              return ret;
            },
            [&capture](StepType it) { //
              ParameterTypeAST type;
              auto ret = it.step(type, ParameterTypeParser<Iterator>());
              if (ret) {
                capture = std::move(type);
              }
              return ret;
            });
  }
};

} // namespace ast

#endif
