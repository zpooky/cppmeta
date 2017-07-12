#ifndef SP_CPP_META_EXPRESSION_PARSER_H
#define SP_CPP_META_EXPRESSION_PARSER_H

#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace stk {
template <typename>
struct ExpressionParser;

template <typename>
struct AssignmentParser;

template <typename>
struct UnaryOpeatorInvocationParser;

template <typename>
struct BinaryOpeatorInvocationParser;

template <typename>
struct TermParser;

template <typename>
struct TurneryParser;

template <typename>
struct TypeCastParser;

/*TypeExpressionParser*/
template <typename Iterator>
struct TypeExpressionParser : match::Base<TypeExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = TypeExpressionAST;

  StepType operator()(capture_type &result, StepType start) const {
    // TODO
    Token e;
    auto ret = start.step(e);
    if (ret) {
      result = TypeExpressionAST(e);
    }
    return ret;
  }
};

/*NumericConstantParser*/
template <typename Iterator>
struct NumericConstantParser : match::Base<NumericConstantAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = NumericConstantAST;

  StepType operator()(capture_type &result, StepType start) const {
    // int i = +111, dd = -22, dkkd = 33, ffkf = 13ul, jfjf = +-11;
    // TODO hex,binary
    Token sign;
    Token nom;
    Token fp;
    Token denom;
    return start                                            //
        .option(sign, match::Either({"-"}))                 //
        .step(nom, ast::NumericPattern<Iterator>())         //
        .option([&fp, &denom](StepType it) {                //
          return it                                         //
              .step(fp, ".")                                //
              .step(denom, ast::NumericPattern<Iterator>()) //
              ;
        }) //
        ;
  }
};

/*StringParser*/
template <typename Iterator>
struct StringParser : match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = Token;

  StepType operator()(capture_type &result, StepType start) const {
    // TODO capture
    // const char *s = "asd"
    //                 " fdsads";
    Token head;
    std::vector<Token> strs;
    return start    //
        .step("\"") //
        .step(head) //
        .step("\"") //
        .repeat(strs,
                [](Token &capture, StepType it) {
                  Token begin;
                  Token str;
                  Token end;
                  auto ret = it                     //
                                 .option("\\")      //
                                 .step(begin, "\"") //
                                 .step(str)         //
                                 .step(end, "\"")   //
                      ;
                  if (ret) {
                    capture = match::join(begin, match::join(str, end));
                  }
                  return ret;
                }) //
        ;
  }
};

/*CharacterParser*/
template <typename Iterator>
struct CharacterParser : match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = Token;

  StepType operator()(capture_type &capture, StepType start) const {
    Token begin;
    Token c;
    Token end;
    auto ret = start                 //
                   .step(begin, "'") //
                   .step(c)          //
                   .step(end, "'")   //
        ;
    if (ret) {
      capture = match::join(begin, match::join(c, end));
    }
    return ret;
  }
};

template <typename Iterator>
struct ExpressionScopeParser : match::Base<ExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ExpressionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    return start                                     //
        .step("(")                                   //
        .step(capture, ExpressionParser<Iterator>()) //
        .step(")");
  }
};

/*FunctionInvocationParser*/
template <typename Iterator>
struct FunctionInvocationParser : match::Base<FunctionInvocationAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionInvocationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    std::vector<Token> ns;
    Token name;
    std::vector<ast::TypeIdentifier> typeArguments;
    std::vector<ExpressionAST> arguments;

    return start                                                  //
        .repeat(ns, ast::NsParser<Iterator>())                    //
        .step(name, ast::FunctionName<Iterator>())                //
        .step(typeArguments, ast::TypeArgumentParser<Iterator>()) //
        .step("(")                                                //
        .repeat(arguments, ExpressionParser<Iterator>(), ",")     //
        .step(")")                                                //
        ;
  }
};

/*TermParser*/
// should describe a single expression eg. '1' or '(1+1)', 'std::var', and not
// '1+1'
template <typename Iterator>
struct TermParser : match::Base<TermAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = TermAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO exp scope parse
    // TODO capture
    return start //
        .eitherx(
            [](StepType it) { //
              NumericConstantAST numCnst;
              return it.step(numCnst, NumericConstantParser<Iterator>());
            },
            [](StepType it) { //
              Token strCnst;
              return it.step(strCnst, StringParser<Iterator>());
            },
            [](StepType it) { //
              Token charCnst;
              return it.step(charCnst, CharacterParser<Iterator>());
            },
            // [](StepType it) { //readd
            //   ExpressionAST scoped;
            //   return it.step(scoped, ExpressionScopeParser<Iterator>());
            // },
            [](StepType it) { //
              Token var;
              return it.step(var, ast::VariableRefParser<Iterator>());
            }
            // ,
            // [](StepType it) { //readd
            //   UnaryOperatorInvocationAST exp;
            //   return it.step(exp, UnaryOpeatorInvocationParser<Iterator>());
            // },                //
            // [](StepType it) { //
            //   TurneryAST exp;
            //   return it.step(exp, TurneryParser<Iterator>());
            // },
            // [](StepType it) { //readd
            //   TypeCastAST exp;
            //   return it.step(exp, TypeCastParser<Iterator>());
            // } //

            ) //
        ;
  }
}; // TermParser

/*TypeCastParser*/
template <typename Iterator>
struct TypeCastParser : match::Base<TypeCastAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = TypeCastAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    ast::TypeIdentifier type;
    TermAST exp;
    return start                                           //
        .step("(")                                         //
        .step(type, ast::TypeIdentifierParser<Iterator>()) //
        .step(exp, TermParser<Iterator>())                 //
        .step(")");                                        //
  }
};

/*TurneryParser*/
template <typename Iterator>
struct TurneryParser : match::Base<TurneryAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = TurneryAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    ExpressionAST condition;
    ExpressionAST trueBranch;
    ExpressionAST falseBranch;
    return start                                         //
        .step(condition, ExpressionParser<Iterator>())   //
        .step("?")                                       //
        .step(trueBranch, ExpressionParser<Iterator>())  //
        .step(":")                                       //
        .step(falseBranch, ExpressionParser<Iterator>()) //
        ;
  }
};

/*OpeatorInvocationParser*/
template <typename Iterator>
struct BinaryOpeatorInvocationParser
    : match::Base<BinaryOperatorInvocationAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = BinaryOperatorInvocationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    using namespace match;
    // TODO
    // capture
    // if(value = 1)
    // if(int value = 1)
    // i / u + 1
    std::vector<String> bin{
        "&&", "||",                  //
        "/",  "*",  "%",  "+",  "-", //
        ">",  "<",  ">=", "<=",      //
        "&",  "|",                   //
        "==", "!="                   //
    };
    TermAST first;
    Token op;
    TermAST second;
    return start                                  //
        .step(first, TermParser<Iterator>())      //
        .step(op, TokenJoinParser<Iterator>(bin)) //
        .step(second, TermParser<Iterator>())     //
        ;
  }
};

/*UnaryOpeatorInvocationParser*/
template <typename Iterator>
struct UnaryOpeatorInvocationParser
    : match::Base<UnaryOperatorInvocationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = UnaryOperatorInvocationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO seg faults
    using namespace match;
    Token op;
    TermAST exp;
    // TODO capture
    return start.eitherx(
        [&op, &exp](StepType it) -> StepType {
          // prefix decrement operator
          return it                                        //
              .step(op, TokenJoinParser<Iterator>({"--"})) //
              .step(exp, TermParser<Iterator>())           //
              ;
        },
        // [&op, &exp](StepType it) {
        //   // postfix decrement operator
        //   return it                                        //
        //       .step(exp, TermParser<Iterator>())           //
        //       .step(op, TokenJoinParser<Iterator>({"--"})) //
        //       ;
        // },
        [&op, &exp](StepType it) {
          // boolean negate opeator
          return it                              //
              .step(op, "!")                     //
              .step(exp, TermParser<Iterator>()) //
              ;
        },
        [&op, &exp](StepType it) {
          // bitwise negate opeator
          return it                              //
              .step(op, "-")                     //
              .step(exp, TermParser<Iterator>()) //
              ;
        },
        [&op, &exp](StepType it) {
          // prefix increment opeator
          return it                                        //
              .step(op, TokenJoinParser<Iterator>({"++"})) //
              .step(exp, TermParser<Iterator>())           //
              ;
        },
        // [&op, &exp](StepType it) {
        //   // postfix increment opeator
        //   return it                                        //
        //       .step(exp, TermParser<Iterator>())           //
        //       .step(op, TokenJoinParser<Iterator>({"++"})) //
        //       ;
        // },
        [&op, &exp](StepType it) {
          // bitwise inverse opeator
          return it                              //
              .step(op, "~")                     //
              .step(exp, TermParser<Iterator>()) //
              ;
        }, //
        [&op, &exp](StepType it) {
          // reference operator
          return it                              //
              .step(op, "&")                     //
              .step(exp, TermParser<Iterator>()) //
              ;
        }, //
        [&op, &exp](StepType it) {
          // dereference operator
          // TODO capture
          Token fderef;
          std::vector<Token> derefs;
          return it                              //
              .step(fderef, "*")                 //
              .repeat(derefs, Either({"*"}))     //
              .step(exp, TermParser<Iterator>()) //
              ;
        } //
        ) //
        ;
  }
};

/*AssignmentParser*/
template <typename Iterator>
struct AssignmentParser : match::Base<ExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ExpressionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    using namespace match;
    std::vector<String> assign{
        "=",                     //
        "+=",  "-=", "*=", "/=", //
        "%=",                    //
        "&=",  "|=", "^=",       //
        "<<=", ">=="             //
    };
    TermAST first;
    Token op;
    TermAST second;
    return start                                     //
        .step(first, TermParser<Iterator>())         //
        .step(op, TokenJoinParser<Iterator>(assign)) //
        .eitherx(
            [&second](StepType it) {
              return it.step(second, TermParser<Iterator>()) //
                  ;
            },
            [](StepType it) {
              // TODO capture
              ExpressionAST second;
              return it.step(second, AssignmentParser<Iterator>()) //
                  ;
            } //
            ) //
        ;
  }
};

/*ExpressionParser*/
template <typename Iterator>
struct ExpressionParser : match::Base<ExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

  template <typename Parser>
  static auto parse(StepType it) {
    using Capture = typename Parser::capture_type;
    Parser parser;
    Capture ast;
    return parser(ast, it);
  }

public:
  using capture_type = ExpressionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    auto it = start;
    // while (it.valid && it.it != it.end) {
    // {
    //   auto res = parse<ExpressionScopeParser<Iterator>>(it);
    //   if (res) {
    //     it = res;
    //     capture.push_back(res);
    //     return it;
    //   }
    // }
    {
      auto res = parse<BinaryOpeatorInvocationParser<Iterator>>(it);
      if (res) {
        it = res;
        capture.push_back(res);
        return it;
      }
    }
    // {
    //   auto res = parse<FunctionInvocationParser<Iterator>>(it);
    //   if (res) {
    //     it = res;
    //     capture.push_back(res);
    //     return it;
    //   }
    // }
    {
      auto res = parse<TermParser<Iterator>>(it);
      if (res) {
        it = res;
        capture.push_back(res);
        return it;
      }
    }
    // {
    //   auto res = parse<AssignmentParser<Iterator>>(it);
    //   if (res) {
    //     it = res;
    //     capture.push_back(res);
    //     // continue;
    //     return it;
    //   }
    // }
    //   break;
    // } // while
    // bool foundMatching = it != start;
    // return StepType(it.it, it.end, foundMatching);
    return StepType(it.it, it.end, false);
  }
};
} // namespace stk

#endif
