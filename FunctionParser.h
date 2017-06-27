#ifndef SP_CPP_META_CLASS_FUNCTION_PARSER_H
#define SP_CPP_META_CLASS_FUNCTION_PARSER_H

#include "ParameterParser.h"
#include "Pattern.h"
#include "TemplateParser.h"
#include "ast.h"
#include "matcher.h"

namespace ast {
template <typename Iterator>
class FunctionDefinitionParser
    : public match::Base<FunctionDefinitionAST, Iterator> {
  using StepType = match::Step<Iterator>;

public:
  using capture_type = FunctionDefinitionAST;

  StepType operator()(capture_type &capture, StepType step) const {
    // TODO
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
    // int f(char* const);
    // auto fp11() -> void(*)(const std::string&)
    std::vector<tmp::TemplateTypenameAST> templates;
    std::vector<Token> prefix;
    ParameterTypeAST returnType;
    Token functionName;
    std::vector<ParameterAST> parameters;
    std::vector<Token> postfix;
    bool pureVirtual = false;

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
                                                   "noexcept"})) //
                   .option([&pureVirtual](StepType it) {         //
                     auto ret = it                               //
                                    .step("=")                   //
                                    .step("0");
                     if (ret) {
                       pureVirtual = true;
                     }
                     return ret;
                   })
                   .step(";") //
        ;
    if (ret) {
      capture =
          FunctionDeclarationAST(templates, prefix, returnType, functionName,
                                 parameters, postfix, pureVirtual);
    }
    return ret;
  }
};

// TODO operator, constructor,destructor
// void* operator new[](std::size_t) = delete;

} // namespace ast

#endif
