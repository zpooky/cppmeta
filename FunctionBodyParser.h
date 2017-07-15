#ifndef SP_CPP_META_FUNCTION_BODY_PARSER_H
#define SP_CPP_META_FUNCTION_BODY_PARSER_H

#include "ExpressionParser.h"
#include "ParameterParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace stk {

template <typename Iterator>
class ScopeParser;

/*IfParser*/
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
              ExpressionAST exp;

              ScopeAST scope;
              return it
                  .step("if")                              //
                  .step("(")                               //
                  .step(exp, ExpressionParser<Iterator>()) //
                  .step(")")                               //
                  .step("{")                               //
                  .step(scope, ScopeParser<Iterator>())    //
                  .step("}");
            },
            "else") //
        .option([](StepType it) {
          ScopeAST scope;
          return it                                 //
              .step("else")                         //
              .step("{")                            //
              .step(scope, ScopeParser<Iterator>()) //
              .step("}");
        });
  }
};

/*ReturnParser*/
template <typename Iterator>
class ReturnParser : public match::Base<ReturnAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ReturnAST;

  StepType operator()(capture_type &capture, StepType start) const {
    ExpressionAST exp;
    auto ret = start                                          //
                   .step("return")                            //
                   .option(exp, ExpressionParser<Iterator>()) //
                   .step(";")                                 //
        ;
    if (ret) {
      capture = capture_type(exp);
    }
    return ret;
  }
};

/*VariableConstructParser*/
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
    ExpressionAST assignment;
    return start                                       //
        .repeat(prefix, match::Either({"&", "*"}))     //
        .step(variable, ast::VariableName<Iterator>()) //
        .eitherx(
            [&assignment](StepType it) {
              auto ret = it                                                  //
                             .step("=")                                      //
                             .step(assignment, ExpressionParser<Iterator>()) //
                  ;
              return ret;
            },
            [](StepType it) { //
              std::vector<ExpressionAST> arguments;
              Token open;
              return it                                                 //
                  .step(open, match::Either({"(", "{"}))                //
                  .repeat(arguments, ExpressionParser<Iterator>(), ",") //
                  .step(open == "(" ? ")" : "}")                        //
                  ;
            }) //
        ;
  }
};

/*VariableDeclarationParser*/
template <typename Iterator>
struct VariableDeclarationParser
    : match::Base<VariableDeclarationAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = VariableDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    // int ix(0), xx = 2, fff{0}, *ff(nullptr), kg, &dd = xx, **iptr = 0;
    ast::ParameterEither type;
    std::vector<VariableConstructorAST> variables;
    return start                                                     //
        .step(type, ast::ParameterEitherParser<Iterator>())          //
        .repeat(variables, VariableConstructParser<Iterator>(), ",") //
        .step(";")                                                   //
        ;
  }
};

/*WhileParser*/
template <typename Iterator>
struct WhileParser : match::Base<WhileAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = WhileAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    ExpressionAST condition;
    ScopeAST scope;
    return start                                       //
        .step("while")                                 //
        .step("(")                                     //
        .step(condition, ExpressionParser<Iterator>()) //
        .step(")")                                     //
        .step("{")                                     //
        .stepx([&scope](StepType it) {                 //
          return it.step(scope, ScopeParser<Iterator>());
        })         //
        .step("}") //
        ;
  }
};

/*DoWhileParser*/
template <typename Iterator>
struct DoWhileParser : match::Base<DoWhileAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = DoWhileAST;

  StepType operator()(capture_type &, StepType start) const {
    // TODO capture
    ExpressionAST condition;
    ScopeAST scope;
    return start                       //
        .step("do")                    //
        .step("{")                     //
        .stepx([&scope](StepType it) { //
          return it.step(scope, ScopeParser<Iterator>());
        })                                             //
        .step("}")                                     //
        .step("while")                                 //
        .step("(")                                     //
        .step(condition, ExpressionParser<Iterator>()) //
        .step(")")                                     //
        ;
  }
};

/*ScopeParser*/
template <typename Iterator>
class ScopeParser : public match::Base<ScopeAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

  template <typename Parser>
  static auto parse(StepType it) {
    using Capture = typename Parser::capture_type;
    Parser parser;
    Capture ast;
    return parser(ast, it);
  }

public:
  using capture_type = ScopeAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // return StepType(start.it, start.end, true);
    auto it = start;
    while (it.valid && it.it != it.end) {
      {
        auto res = parse<WhileParser<Iterator>>(it);
        if (res) {
          it = res;
          capture.push_back(res);
          continue;
        }
      }
      {
        auto res = parse<DoWhileParser<Iterator>>(it);
        if (res) {
          it = res;
          capture.push_back(res);
          continue;
        }
      }
      // {
      //   auto res = parse<IfParser<Iterator>>(it);
      //   if (res) {
      //     it = res;
      //     capture.push_back(res);
      //     continue;
      //   }
      // }
      {
        auto res = parse<VariableDeclarationParser<Iterator>>(it);
        if (res) {
          it = res;
          capture.push_back(res);
          continue;
        }
      }
      {
        auto res = parse<ReturnParser<Iterator>>(it);
        if (res) {
          it = res;
          capture.push_back(res);
          continue;
        }
      }
      {
        auto res = parse<ExpressionParser<Iterator>>(it);
        if (res) {
          it = res;
          capture.push_back(res);
          continue;
        }
      }
      break;
    } // while
    return it;
  }
};

// TODO
// turnery
// for
// switch
// operator invocation(implict&explicit)
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
// new (int (*[10])());
// delete ([]{return new int; })();
// support scopes without {}. ex. if while for

} // namspeace ast

#endif
