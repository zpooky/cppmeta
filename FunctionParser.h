#ifndef SP_CPP_META_CLASS_FUNCTION_PARSER_H
#define SP_CPP_META_CLASS_FUNCTION_PARSER_H

#include "FunctionBodyParser.h"
#include "ParameterParser.h"
#include "Pattern.h"
#include "TemplateParser.h"
#include "ast.h"
#include "matcher.h"

namespace {
// TODO throw(,...)
// constructor() : wtf(dd) {}
// all function&operator = default/delete;

/*==================================================================*/
/*MemberDeclarationClassRefParser*/
template <typename Iterator>
class MemberDeclRefParser : public match::Base<ast::TypeIdentifier, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = ast::TypeIdentifier;

  StepType operator()(capture_type &capture, StepType start) const {
    capture_type tmpCapture;

    auto ret = start                                                        //
                   .step(tmpCapture, ast::TypeIdentifierParser<Iterator>()) //
                   .step("::");
    if (ret) {
      capture = tmpCapture;
    }

    return ret;
  }
};

/*==================================================================*/
template <typename Iterator>
match::Step<Iterator> functionPrototype(ast::FunctionDefinitionAST &capture,
                                        match::Step<Iterator> start) {
  using StepType = match::Step<Iterator>;
  using namespace match;
  // TODO
  // int f(int a = 7, int *p = nullptr, int (*(*x)(double))[3] = nullptr);
  // int printf(const char* fmt, ...);
  // auto fp11() -> void(*)(const std::string&)
  std::vector<tmp::TemplateTypenameAST> templates;
  std::vector<Token> prefix;
  ast::ReturnTypeAST returnType;
  Token functionName;
  std::vector<ast::ParameterAST> parameters;
  std::vector<Token> postfix;
  bool pureVirtual = false;
  bool deleted = false;

  // TODO capture memDeclRef
  ast::TypeIdentifier memDeclRef;

  auto ret = //
      start  //
          .option(templates, ast::TemplateParser<Iterator>()) //
          .repeat(prefix, Either({"constexpr", "friend", "inline", "virtual",
                                  "static", "extern"}))        //
          .step(returnType, ast::ReturnTypeParser<Iterator>()) //
          .option(memDeclRef, MemberDeclRefParser<Iterator>())            //
          .step(functionName, ast::FunctionName<Iterator>())         //
          .step("(")                                                 //
          .repeat(parameters, ast::ParameterParser<Iterator>(), ",") //
          .step(")")                                                 //
          .repeat(postfix, match::Either({"final", "const", "override",
                                          "noexcept", "try"})) //
          .option([&pureVirtual, &deleted](StepType it) {      //
            Token end;
            auto ret = it             //
                           .step("=") //
                           .step(end, match::Either({"0", "delete"}));
            if (ret) {
              pureVirtual = end == "0";
              deleted = end == "delete";
            }
            return ret;
          });
  if (ret) {
    capture =
        ast::FunctionDefinitionAST(templates, prefix, returnType, functionName,
                                   parameters, postfix, pureVirtual, deleted);
  }
  return ret;
} // functionPrototype()
} // namespace

namespace ast {

/*FunctionDefintionParser*/
template <typename Iterator>
class FunctionDefinitionParser
    : public match::Base<FunctionDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    return functionPrototype(capture, start).step(";");
  }
};

/*FunctionDeclarationParser*/
template <typename Iterator>
class FunctionDeclarationParser
    : public match::Base<FunctionDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
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
    // TODO caputre
    ast::FunctionDefinitionAST def;
    stk::ScopeAST body;
    stk::ScopeAST catchBody;

    auto prototype =
        functionPrototype(def, start) //
            .step("{")
            .step(body, stk::ScopeParser<Iterator>()) //
            .step("}")                                //
            .option([&catchBody](StepType catchIt) {  //
              // catch( ... ){ ... }
              // TODO capture
              ParameterAST catchParam;
              return catchIt                                          //
                  .step("catch")                                      //
                  .step("(")                                          //
                  .step(catchParam, ast::ParameterParser<Iterator>()) //
                  .step(")")                                          //
                  .step("{")                                          //
                  .step(catchBody, stk::ScopeParser<Iterator>())      //
                  .step("}");
            }) //
        ;
    if (prototype) {
    }
    return prototype;
  }
};
} // namespace ast

/*==================================================================*/
namespace ast {
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
}; // class OperatorTypeParser
} // namespace ast

namespace {
template <typename Iterator>
match::Step<Iterator> operatorPrototype(ast::OperatorDefinitionAST &capture,
                                        match::Step<Iterator> start) {
  using namespace match;
  using StepType = match::Step<Iterator>;

  // TODO capture
  std::vector<tmp::TemplateTypenameAST> templates;
  Token virtualOp;
  Token operatorType;
  ast::ReturnTypeAST returnType;
  ast::TypeIdentifier memDeclRef;
  std::vector<ast::ParameterAST> paramters;
  std::vector<Token> postfix;

  bool pureVirtual = false;
  bool deleted = false;
  bool defaulted = false;

  return start                                                             //
      .option(templates, ast::TemplateParser<Iterator>())                  //
      .option(virtualOp, Either({"constexpr", "virtual"}))                 //
      .step(returnType, ast::ReturnTypeParser<Iterator>())                 //
      .option(memDeclRef, MemberDeclRefParser<Iterator>())                 //
      .step(operatorType, ast::OperatorTypeParser<Iterator>())             //
      .step("(")                                                           //
      .repeat(paramters, ast::ParameterParser<Iterator>(), ",")            //
      .step(")")                                                           //
      .repeat(postfix, Either({"final", "const", "override", "noexcept"})) //
      .option([&pureVirtual, &deleted, &defaulted](StepType it) {          //
        Token end;
        auto ret = it             //
                       .step("=") //
                       .step(end, Either({"0", "delete", "default"}));
        if (ret) {
          pureVirtual = end == "0";
          deleted = end == "delete";
          defaulted = end == "default";
        }
        return ret;
      });
} // operatorPrototype()
} // namespace

namespace ast {
/*OperatorDefinitionParser*/
template <typename Iterator>
class OperatorDefinitionParser
    : public match::Base<OperatorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = OperatorDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    return operatorPrototype(capture, start).step(";");
  }
};

/*OperatorDeclarationParser*/
template <typename Iterator>
class OperatorDeclarationParser
    : public match::Base<OperatorDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = OperatorDeclarationAST;

  StepType operator()(capture_type &, StepType start) const {
    // TODO capture
    OperatorDefinitionAST defAST;
    return                               //
        operatorPrototype(defAST, start) //
            .option([](StepType it) {    //
              // TODO
              return it      //
                  .step(":") //
                  ;
            })                          //
            .step("{")                  //
            .stepx([](StepType start) { //
              stk::ScopeAST stackAST;
              return start //
                  .step(stackAST, stk::ScopeParser<Iterator>());
            }) //
            .step("}");
  }
};

} // namespace ast
/*==================================================================*/
namespace ast {
/*CtorDefinitionParser*/
template <typename Iterator>
class CtorDefinitionParser : public match::Base<CtorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = CtorDefinitionAST;

  StepType operator()(capture_type &, StepType start) const {
    // TODO capture
    ast::TypeIdentifier memDeclRef;
    Token ctorType;
    std::vector<ast::ParameterAST> parameters;
    return start //
        // .option(memDeclRef, MemberDeclRefParser<Iterator>())       //
        .step(ctorType, ast::TypeName<Iterator>())                 //
        .step("(")                                                 //
        .repeat(parameters, ast::ParameterParser<Iterator>(), ",") //
        .step(")")                                                 //
        .step(";");
  }
};

/*CtorDeclarationParser*/
template <typename Iterator>
class CtorDeclarationParser : public match::Base<CtorDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = CtorDeclarationAST;

  StepType operator()(capture_type &, StepType start) const {
    // TODO capture
    std::vector<tmp::TemplateTypenameAST> templates;
    TypeIdentifier memDeclRef;
    Token ctorType;
    std::vector<ParameterAST> parameters;

    return start
        .option(templates, TemplateParser<Iterator>())             //
        .option(memDeclRef, MemberDeclRefParser<Iterator>())       //
        .step(ctorType, TypeName<Iterator>())                      //
        .step("(")                                                 //
        .repeat(parameters, ast::ParameterParser<Iterator>(), ",") //
        .step(")")                                                 //
        .step("{")                                                 //
        .stepx([](StepType start) {                                //
          stk::ScopeAST stackAST;
          return start //
              .step(stackAST, stk::ScopeParser<Iterator>());
        }) //
        .step("}");
  }
};

} // namespace ast
/*==================================================================*/
namespace ast {
/*DtorDefinitionParser*/
template <typename Iterator>
class DtorDefinitionParser : public match::Base<DtorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = DtorDefinitionAST;

  StepType operator()(capture_type &, StepType start) const {
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

  StepType operator()(capture_type &, StepType start) const {
    // TODO capture
    std::vector<tmp::TemplateTypenameAST> templates;
    TypeIdentifier memDeclRef;
    Token dtorType;
    std::vector<ParameterAST> parameters;

    return start                                             //
        .option(templates, TemplateParser<Iterator>())       //
        .option(memDeclRef, MemberDeclRefParser<Iterator>()) //
        .step("~")                                           //
        .step(dtorType, TypeName<Iterator>())                //
        .step("(")                                           //
        .step(")")                                           //
        .step("{")                                           //
        .stepx([](StepType start) {                          //
          stk::ScopeAST stackAST;
          return start //
              .step(stackAST, stk::ScopeParser<Iterator>());
        }) //
        .step("}");
  }
};

} // namespace ast

#endif
