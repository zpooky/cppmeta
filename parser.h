#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ArrayList.h"
#include "ClassParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"
#include "tokens.h"
#include "FunctionParser.h"
#include "EnumParser.h"

namespace {
template <typename Iterator, typename AST>
match::Step<Iterator> generic_scope(AST &, match::Step<Iterator>);
}

namespace ast {
// ex: using namespace Type;
// ex:: using ns::ns::Type;
template <typename Iterator>
class UsingNamespaceParser : public match::Base<UsingNamespaceAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(UsingNamespaceAST &capture, StepType start) const {
    std::vector<Token> nss;
    auto ret =  start
        .step("using")                                //
        .step("namespace")                            //
        .repeat(nss, NamespaceName<Iterator>(), "::") //
        .step(";");

    if(ret){
      capture = UsingNamespaceAST(nss);
    }
    return ret;
  }
};

// ex:: using ns::ns::Type;
template <typename Iterator>
class UsingTypeParser : public match::Base<UsingTypeAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(UsingTypeAST &capture, StepType start) const {
    TypeIdentifier alias;
    auto ret = start
        .step("using")                                 //
        .step(alias, TypeIdentifierParser<Iterator>()) //
        .step(";");

    if(ret){
      capture = UsingTypeAST(alias);
    }
    return ret;
  }
};

// ex:
// template<tyename T>
// using Alias = Concrete<T>;
template <typename Iterator>
class UsingAliasParser : public match::Base<UsingAliasAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(UsingAliasAST &capture, StepType start) const {
    std::vector<tmp::TemplateTypenameAST> templates;
    TypeIdentifier alias;
    TypeIdentifier concrete;
    auto ret = start
        .option(templates, TemplateParser<Iterator>())    //
        .step("using")                                    //
        .step(alias, TypeIdentifierParser<Iterator>())    //
        .step("=")                                        //
        .step(concrete, TypeIdentifierParser<Iterator>()) //
        .step(";");

    if(ret){
      capture = UsingAliasAST(templates,alias,concrete);
    }
    return ret;
  }
};

// ex: namespace ns { ... }
template <typename Iterator>
class NamespaceParser : public match::Base<NamespaceAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  NamespaceParser() {
  }

  StepType operator()(NamespaceAST &capture, StepType start) const {
    Token ns;
    auto s = start
                 .step("namespace")                     //
                 .option(ns, NamespaceName<Iterator>()) //
                 .step("{");                            //
    if (s) {
      capture = NamespaceAST(ns);
      return generic_scope(capture, s) //
          .step("}");
    }
    return s;
  }
};

class Parser {
public:
  Parser() {
  }

  template <typename Iterator>
  match::Step<Iterator> parse(FileAST &result, match::Step<Iterator> start) {
    return generic_scope(result, start);
  } // Parse::parse
};
}

namespace {
template <typename Iterator, typename AST>
match::Step<Iterator> generic_scope(AST &result, match::Step<Iterator> start) {
  while (start.it != start.end) {
    {
      ast::ClassAST ast;
      auto next = start.step(ast, ast::ClassParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::EnumAST ast;
      auto next = start.step(ast, ast::EnumParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::NamespaceAST ast;
      auto next = start.step(ast, ast::NamespaceParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::UsingNamespaceAST ast;
      auto next = start.step(ast, ast::UsingNamespaceParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::UsingAliasAST ast;
      auto next = start.step(ast, ast::UsingAliasParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::UsingTypeAST ast;
      auto next = start.step(ast, ast::UsingTypeParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::FunctionDefinitionAST ast;
      auto next = start.step(ast, ast::FunctionDefinitionParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    return start;
  }
  return start;
}
}

#endif
