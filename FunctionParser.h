#ifndef SP_CPP_META_CLASS_FUNCTION_PARSER_H
#define SP_CPP_META_CLASS_FUNCTION_PARSER_H

#include "ParameterParser.h"
#include "Pattern.h"
#include "TemplateParser.h"
#include "ast.h"
#include "matcher.h"

namespace ast {

/*FunctionDefinitionParser*/
template <typename Iterator>
class FunctionDefinitionParser
    : public match::Base<FunctionDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDefinitionAST;

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

/*FunctionDeclarationParser*/
template <typename Iterator>
class FunctionDeclarationParser
    : public match::Base<FunctionDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDeclarationAST;

  StepType operator()(capture_type &capture, StepType step) const {
    // TODO
    // int f(int a = 7, int *p = nullptr, int (*(*x)(double))[3] = nullptr);
    // int printf(const char* fmt, ...);
    // auto fp11() -> void(*)(const std::string&)
    // = delete;
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
          FunctionDeclarationAST(templates, prefix, returnType, functionName,
                                 parameters, postfix, pureVirtual, deleted);
    }
    return ret;
  }
};

// TODO operator, constructor,destructor
// void* operator new[](std::size_t) = delete;

/*OperatorDefinitionParser*/
template <typename Iterator>
class OperatorDeclarationParser
    : public match::Base<OperatorDeclarationAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = OperatorDeclarationAST;

  StepType operator()(capture_type &capture, StepType start) const {
    std::vector<tmp::TemplateTypenameAST> templates;
    Token virtualOp;
    ParameterTypeAST returnType;
    std::vector<ParameterAST> paramters;
    std::vector<Token> postfix;
    // TODO create ast
    return start                                           //
        .option(templates, TemplateParser<Iterator>())     //
        .option(virtualOp, "virtual")                      //
        .step(returnType, ParameterTypeParser<Iterator>()) //
        .step("operator")                                  //
        .step("[")
        .step("]")                                           //
        .step("(")                                           //
        .repeat(paramters, ParameterParser<Iterator>(), ",") //
        .step(")")                                           //
        .repeat(postfix,
                match::Either({"final", "const", "override", "noexcept"})) //
        .step(";");
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
    // TODO
    return StepType(start.it, start.end, false);
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

/*CtorDefinitionParser*/
template <typename Iterator>
class CtorDefinitionParser : public match::Base<CtorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = CtorDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    return StepType(start.it, start.end, false);
  }
};

/*DtorDefinitionParser*/
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

/*DtorDefinitionParser*/
template <typename Iterator>
class DtorDefinitionParser : public match::Base<DtorDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = DtorDefinitionAST;

  StepType operator()(capture_type &capture, StepType start) const {
    // TODO
    return StepType(start.it, start.end, false);
  }
};

} // namespace ast

#endif
