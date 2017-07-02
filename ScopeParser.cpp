#include "ScopeParser.h"

#include "ClassParser.h"
#include "EnumParser.h"
#include "MemberParser.h"
#include "parser.h"

namespace ast {
match::Step<sp::ArrayList<Token>::const_iterator> generic_scope( //
    ScopeAST &result,                                            //
    match::Step<sp::ArrayList<Token>::const_iterator> start      //
    ) {
  using Iterator = sp::ArrayList<Token>::const_iterator;
  while (start.it != start.end) {
    // TODO
    // match::Either scopes({"public", "private", "protected"});
    // Token scope;
    // auto scopeStart = it                       //
    //                       .step(scope, scopes) //
    //                       .step(":");
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
    /*Function*/
    {
      ast::FunctionDefinitionAST ast;
      auto next = start.step(ast, ast::FunctionDefinitionParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::FunctionDeclarationAST ast;
      auto next = start.step(ast, ast::FunctionDeclarationParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    /*operator*/
    {
      ast::OperatorDeclarationAST ast;
      auto next = start.step(ast, ast::OperatorDeclarationParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::OperatorDefinitionAST ast;
      auto next = start.step(ast, ast::OperatorDefinitionParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    /*ctor*/
    {
      ast::CtorDefinitionAST ast;
      auto next = start.step(ast, ast::CtorDefinitionParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::CtorDeclarationAST ast;
      auto next = start.step(ast, ast::CtorDeclarationParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    /*dtor*/
    {
      ast::DtorDefinitionAST ast;
      auto next = start.step(ast, ast::DtorDefinitionParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::DtorDeclarationAST ast;
      auto next = start.step(ast, ast::DtorDeclarationParser<Iterator>());
      if (next.valid) {
        result.push_back(ast);
        start = next;
        continue;
      }
    }
    {
      ast::MemberDefinitionAST ast;
      auto next = start.step(ast, ast::MemberDefinitionParser<Iterator>());
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
} // namespace ast
