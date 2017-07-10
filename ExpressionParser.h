#ifndef SP_CPP_META_EXPRESSION_PARSER_H
#define SP_CPP_META_EXPRESSION_PARSER_H

#include "Pattern.h"
#include "ast.h"
#include "matcher.h"

namespace stk {
template <typename Iterator>
struct ExpressionParser;

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

template <typename Iterator>
struct NumericConstantParser : match::Base<NumericConstantAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = NumericConstantAST;

  StepType operator()(capture_type &result, StepType start) const {
    // int i = +111, dd = -22, dkkd = 33, ffkf = 13ul, jfjf = +-11;
    Token sign;
    Token nom;
    Token fp;
    Token denom;
    return start                                            //
        .option(sign, match::Either({"-", "+"}))            //
        .step(nom, ast::NumericPattern<Iterator>())         //
        .optionx([&fp, &denom](StepType it) {               //
          return it                                         //
              .step(fp, ".")                                //
              .step(denom, ast::NumericPattern<Iterator>()) //
              ;
        });
  }
};

template <typename Iterator>
struct StringParser : match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = Token;

  StepType operator()(capture_type &result, StepType start) const {
    // TODO capture
    // const char *s = "asd"
    //                 " fdsads";
    std::vector<Token> strs;
    return start //
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
                }) //
        ;
  }
};

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

template <typename Iterator>
struct OpeatorInvocationParser : match::Base<OperatorInvocationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = OperatorInvocationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    using namespace match;
    // TODO
    // capture
    // if(value = 1)
    // if(int value = 1)
    // i / u + 1
    Either bin({
        "&&", "||",              //
        "/", "*", "%", "+", "-", //
        ">", "<",                //
        "&", "|",                //
        "==", "!="               //
    });
    // term bin term
    return start;
  }
};

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

template <typename Iterator>
struct ExpressionParser : match::Base<ExpressionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ExpressionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    Token e;
    auto ret = start.step(e, ast::VariableNameRef<Iterator>());
    if (ret) {
      capture = ExpressionAST({e});
    }
    return ret;
  }
};
} // namespace stk

#endif
