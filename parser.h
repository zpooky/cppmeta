#ifndef SP_CPP_META_PARSER_H
#define SP_CPP_META_PARSER_H

#include "ArrayList.h"
#include "ClassParser.h"
#include "Pattern.h"
#include "ast.h"
#include "matcher.h"
#include "tokens.h"

namespace {
template <typename Iterator, typename AST>
match::Step<Iterator> generic_scope_parse(Iterator, Iterator, AST &);
}

namespace ast {

template <typename Iterator>
class NamespaceParser {
private:
  using StepType = match::Step<Iterator>;

public:
  NamespaceParser() {
  }

  StepType parse(Iterator begin, Iterator end, NamespaceAST &capture) {
    Token ns;
    auto s = match::start(begin, end)                   //
                 .step("namespace")                     //
                 .option(ns, NamespaceName<Iterator>()) //
                 .step("{");                            //
    if (s) {
      capture = NamespaceAST(ns);
      return generic_scope_parse(s.it, s.end, capture) //
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
  match::Step<Iterator> parse(Iterator it, Iterator end, FileAST &result) {
    return generic_scope_parse(it, end, result);
  } // Parse::parse
};
}

namespace {
template <typename Iterator, typename AST>
match::Step<Iterator> generic_scope_parse(Iterator it, Iterator end,
                                          AST &result) {
  while (it != end) {
    auto begin = it++;

    {
      ast::ClassParser<Iterator> parser;
      ast::ClassAST ast;
      match::Step<Iterator> next = parser.parse(begin, end, ast);
      if (next.valid) {
        result.push_back(ast);
        it = next;
        continue;
      }
    }
    {
      ast::NamespaceParser<Iterator> parser;
      ast::NamespaceAST ast;
      match::Step<Iterator> next = parser.parse(begin, end, ast);
      if (next.valid) {
        result.push_back(ast);
        it = next;
        continue;
      }
    }
    return match::Step<Iterator>(begin, end, true);
  }
  return match::Step<Iterator>(it, end, true);
}
}

#endif
