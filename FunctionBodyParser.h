#ifndef SP_CPP_META_FUNCTION_BODY_PARSER_H
#define SP_CPP_META_FUNCTION_BODY_PARSER_H

#include "ExpressionParser.h"
#include "ParameterParser.h"
#include "Pattern.h"
#include "matcher.h"
#include "scope_ast.h"

namespace stk {

template <typename Iterator>
class ScopeParser;

template <typename Iterator>
class IfParser : public match::Base<IfAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = IfAST;

  StepType operator()(capture_type &capture, StepType start) const {

    return start //
        .repeat(
            [](StepType it) {
              ast::ExpressionAST exp;

              ScopeAST scope;
              return it
                  .step("if")                                   //
                  .step("(")                                    //
                  .step(exp, ast::ExpressionParser<Iterator>()) //
                  .step(")")                                    //
                  .step("{")                                    //
                  .step(scope, ScopeParser<Iterator>())         //
                  .step("}");
            },
            "else") //
        .optionx([](StepType it) {
          ScopeAST scope;
          return it                                 //
              .step("else")                         //
              .step("{")                            //
              .step(scope, ScopeParser<Iterator>()) //
              .step("}");
        });
  }
};

template <typename Iterator>
class ArgumentParser : public match::Base<ArgumentAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ArgumentAST;

  StepType operator()(capture_type &capture, StepType start) const {
    return start;
  }
};

template <typename Iterator>
class FunctionInvocationParser
    : public match::Base<FunctionInvocationAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionInvocationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    std::vector<Token> ns;
    Token name;
    std::vector<ast::TypeIdentifier> typeArguments;
    std::vector<ArgumentAST> arguments;

    return start                                                  //
        .repeat(ns, ast::NsParser<Iterator>())                    //
        .step(name, ast::FunctionName<Iterator>())                //
        .step(typeArguments, ast::TypeArgumentParser<Iterator>()) //
        .step("(")                                                //
        .repeat(arguments, ArgumentParser<Iterator>())            //
        .step(")")                                                //
        ;
    // return start;
  }
};

template <typename Iterator>
class ReturnParser : public match::Base<ReturnAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ReturnAST;

  StepType operator()(capture_type &capture, StepType start) const {
    ast::ExpressionAST exp;
    auto ret = start               //
                   .step("return") //
                   .option(exp, ast::ExpressionParser<Iterator>());
    if (ret) {
      capture = capture_type(exp);
    }
    return ret;
  }
};

template <typename Iterator>
class ScopeParser : public match::Base<ScopeAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ScopeAST;

  StepType operator()(capture_type &capture, StepType start) const {
    return StepType(start.it, start.end, true);
    // while(true){
    // }
    // return start;
  }
};

// ex: variable = 1+1
template <typename Iterator>
struct VariableConstructParser : match::Base<VariableConstructorAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = VariableConstructorAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    std::vector<Token> prefix;
    Token variable;
    ast::ExpressionAST assignment;
    return start                                       //
        .repeat(prefix, match::Either({"&", "*"}))     //
        .step(variable, ast::VariableName<Iterator>()) //
        .either(
            [&assignment](StepType it) {
              return it                                                //
                  .step("=")                                           //
                  .step(assignment, ast::ExpressionParser<Iterator>()) //
                  ;
            },
            [](StepType it) { //
              std::vector<ast::ExpressionAST> arguments;
              Token open;
              return it                                                      //
                  .step(open, match::Either({"(", "{"}))                     //
                  .repeat(arguments, ast::ExpressionParser<Iterator>(), ",") //
                  .step(open == "(" ? ")" : "}")                             //
                  ;
            }) //
        ;
  }
};

template <typename Iterator>
struct VariableDeclaration : match::Base<VariableDeclarationAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = VariableDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    int ix(0), xx = 2, fff{0}, *ff(nullptr), kg, &dd = xx, **iptr = 0;
    ast::ParameterEither type;
    std::vector<VariableConstructorAST> variables;
    return start                                                     //
        .step(type, ast::ParameterEitherParser<Iterator>())          //
        .repeat(variables, VariableConstructParser<Iterator>(), ",") //
        .step(";")                                                   //
        ;
  }
};
// TODO turnery
// while
// for
// do
// switch
// operator invocation(implict&explicit)
// member function invocation
// chained function invocation
//(&(*function()))->invoce()
// randomly occuring (), ex: (1+1) = 1+1
// casting
// array init type name[] = {*i,1,&a,function()};
//[using,struct,...] in stack scope
// lambda
// new&new[]
// delete,delete[]
// in place operator new
// enum reference ala EnumType::EnumValue
// array subscript ala var[idx]
// floating point notation 0.0fd
// int notation 0l
// string notation ""
// litteral support L""
// throw exception

} // namspeace ast

#endif
