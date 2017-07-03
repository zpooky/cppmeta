#ifndef SP_CPP_META_CLASS_FUNCTION_PARSER_H
#define SP_CPP_META_CLASS_FUNCTION_PARSER_H

#include "ParameterParser.h"
#include "Pattern.h"
#include "TemplateParser.h"
#include "ast.h"
#include "matcher.h"

namespace ast {

/*==================================================================*/
/*FunctionDefintionParser*/
template <typename Iterator>
class FunctionDefinitionParser
    : public match::Base<FunctionDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDefinitionAST;

  StepType operator()(capture_type &capture, StepType step) const {
    // TODO
    // int f(int a = 7, int *p = nullptr, int (*(*x)(double))[3] = nullptr);
    // int printf(const char* fmt, ...);
    // auto fp11() -> void(*)(const std::string&)
    std::vector<tmp::TemplateTypenameAST> templates;
    std::vector<Token> prefix;
    ParameterTypeAST returnType;
    Token functionName;
    std::vector<ParameterAST> parameters;
    std::vector<Token> postfix;
    bool pureVirtual = false;
    bool deleted = false;

    auto ret = step                                               //
                   .option(templates, TemplateParser<Iterator>()) //
                   .repeat(prefix, match::Either({"friend", "inline", "virtual",
                                                  "static", "extern"}))  //
                   .step(returnType, ParameterTypeParser<Iterator>())    //
                   .step(functionName, FunctionName<Iterator>())         //
                   .step("(")                                            //
                   .repeat(parameters, ParameterParser<Iterator>(), ",") //
                   .step(")")                                            //
                   .repeat(postfix, match::Either({"final", "const", "override",
                                                   "noexcept"}))   //
                   .option([&pureVirtual, &deleted](StepType it) { //
                     Token end;
                     auto ret = it             //
                                    .step("=") //
                                    .step(end, match::Either({"0", "delete"}));
                     if (ret) {
                       pureVirtual = end == "0";
                       deleted = end == "delete";
                     }
                     return ret;
                   })
                   .step(";") //
        ;
    if (ret) {
      capture =
          FunctionDefinitionAST(templates, prefix, returnType, functionName,
                                parameters, postfix, pureVirtual, deleted);
    }
    return ret;
  }
};

/*FunctionDeclarationParser*/
template <typename Iterator>
class FunctionDeclarationParser
    : public match::Base<FunctionDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDeclarationAST;

  StepType operator()(capture_type &capture, StepType step) const {
    // TODO
    // - void class::method()
    // int f2(std::string str) noexcept try
    // {
    //     return std::stoi(str);
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << "stoi() failed!\n";
    //     return 0;
    // }
    return StepType(step.it, step.end, false);
  }
};

/*==================================================================*/
// TODO operator, constructor,destructor
template <typename Iterator>
class OperatorTypeParser : public match::Base<Token, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = Token;

  StepType operator()(capture_type &capture, StepType start) const {
    using namespace match;
    Token base;
    Token tail;
    // TODO void* operator new[](std::size_t) = delete;
    auto ret =
        start //
            .step(base, "operator")
            .eitherx(
                [&tail](StepType it) { //
                  std::vector<String> op{
                      "+",   "-",  "*",   "/",  "%",  "ˆ",  "&",  "|",
                      "~",   "!",  "=",   "<",  ">",  "+=", "-=", "*=",
                      "/=",  "%=", "ˆ=",  "&=", "|=", "<<", ">>", ">>=",
                      "<<=", "==", "!=",  "<=", ">=", "&&", "||", "++",
                      "--",  ",",  "->*", "->", "()", "[]"};
                  // operator
                  return it.step(tail, match::TokenJoinParser<Iterator>(op));
                },
                [](StepType it) {
                  // operator ""
                  // TODO capture
                  Token literal;
                  return it          //
                      .step("\"")    //
                      .step(literal) //
                      .step("\"")    //
                      ;

                },
                [](StepType it) {
                  // cast operator
                  // "operator type"
                  // TODO capture
                  TypeIdentifier type;
                  return it //
                      .step(type, TypeIdentifierParser<Iterator>());

                },
                [](StepType it) {
                  // operator delete
                  // operator delete []
                  // operator new
                  // operator new []
                  // TODO capture
                  Token t;
                  return it //
                      .step(t, Either({"delete", "new"}))
                      .option([](StepType it) {
                        // TODO capture
                        return it      //
                            .step("[") //
                            .step(")");
                      });
                });

    if (ret) {
      capture = std::move(match::join(base, tail));
    }
    return ret;
  }
};

/*OperatorDefinitionParser*/
template <typename Iterator>
class OperatorDefinitionParser
    : public match::Base<OperatorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = OperatorDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    using namespace match;
    std::vector<tmp::TemplateTypenameAST> templates;
    Token virtualOp;
    Token operatorType;
    ParameterTypeAST returnType;
    std::vector<ParameterAST> paramters;
    std::vector<Token> postfix;
    bool pureVirtual = false;
    bool deleted = false;
    // TODO create ast
    return start                                                             //
        .option(templates, TemplateParser<Iterator>())                       //
        .option(virtualOp, "virtual")                                        //
        .step(returnType, ParameterTypeParser<Iterator>())                   //
        .step(operatorType, OperatorTypeParser<Iterator>())                  //
        .step("(")                                                           //
        .repeat(paramters, ParameterParser<Iterator>(), ",")                 //
        .step(")")                                                           //
        .repeat(postfix, Either({"final", "const", "override", "noexcept"})) //
        .option([&pureVirtual, &deleted](StepType it) {                      //
          Token end;
          auto ret = it             //
                         .step("=") //
                         .step(end, Either({"0", "delete"}));
          if (ret) {
            pureVirtual = end == "0";
            deleted = end == "delete";
          }
          return ret;
        }) //
        .step(";");
  }
};

/*OperatorDeclarationParser*/
template <typename Iterator>
class OperatorDeclarationParser
    : public match::Base<OperatorDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = OperatorDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    return StepType(start.it, start.end, false);
  }
};

/*==================================================================*/
/*CtorDefinitionParser*/
template <typename Iterator>
class CtorDefinitionParser : public match::Base<CtorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = CtorDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    Token ctorType;
    std::vector<ParameterAST> parameters;
    return start                                              //
        .step(ctorType, TypeName<Iterator>())                 //
        .step("(")                                            //
        .repeat(parameters, ParameterParser<Iterator>(), ",") //
        .step(")")                                            //
        .step(";");
  }
};

/*CtorDeclarationParser*/
template <typename Iterator>
class CtorDeclarationParser : public match::Base<CtorDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = CtorDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    return StepType(start.it, start.end, false);
  }
};

/*==================================================================*/
/*DtorDefinitionParser*/
template <typename Iterator>
class DtorDefinitionParser : public match::Base<DtorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = DtorDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO capture
    Token dtorType;
    return start                              //
        .step("~")                            //
        .step(dtorType, TypeName<Iterator>()) //
        .step("(")                            //
        .step(")")                            //
        .step(";");
  }
};

/*DtorDeclarationParser*/
template <typename Iterator>
class DtorDeclarationParser : public match::Base<DtorDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = DtorDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    return StepType(start.it, start.end, false);
  }
};

} // namespace ast

#endif
