#ifndef SP_CPP_META_SCOPE_AST_H
#define SP_CPP_META_SCOPE_AST_H

#include "ast.h"
#include "tokens.h"

namespace stk {

struct ScopeAST { //
};

struct IfAST {
  ast::ExpressionAST expression;
  ScopeAST scope;
};

struct ReturnAST {
  ast::ExternalAST exp;
};

struct FunctionInvocationAST {

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

struct ArgumentAST {
};

struct VariableConstructorAST{
};

struct VariableDeclarationAST{
};

}//namespace stk

#endif
