#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ArrayList.h"
#include "ClassParser.h"
#include "EnumParser.h"
#include "FunctionParser.h"
#include "Pattern.h"
#include "ScopeParser.h"
#include "ast.h"
#include "matcher.h"
#include "tokens.h"

namespace ast {
// ex: using namespace Type;
// ex:: using ns::ns::Type;
template <typename Iterator>
class UsingNamespaceParser : public match::Base<UsingNamespaceAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  using capture_type = UsingNamespaceAST;

  StepType operator()(UsingNamespaceAST &capture, StepType start) const {
    std::vector<Token> nss;
    auto ret = start
                   .step("using")                                //
                   .step("namespace")                            //
                   .repeat(nss, NamespaceName<Iterator>(), "::") //
                   .step(";");

    if (ret) {
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
  using capture_type = UsingTypeAST;

  StepType operator()(UsingTypeAST &capture, StepType start) const {
    TypeIdentifier alias;
    auto ret = start
                   .step("using")                                 //
                   .step(alias, TypeIdentifierParser<Iterator>()) //
                   .step(";");

    if (ret) {
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
  using capture_type = UsingAliasAST;

  StepType operator()(capture_type &capture, StepType start) const {
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

    if (ret) {
      capture = UsingAliasAST(templates, alias, concrete);
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
  using capture_type = NamespaceAST;

  StepType operator()(capture_type &capture, StepType start) const {
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

#endif
