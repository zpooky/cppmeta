#ifndef SP_CPP_META_SCOPE_AST_H
#define SP_CPP_META_SCOPE_AST_H

#include "ast.h"
#include "tokens.h"

namespace stk {

struct ScopeAST { //
  template <typename T>
  ScopeAST(T &&) {
  }

  ScopeAST() {
  }

  template <typename T>
  void push_back(T &&) {
  }
};

struct IfAST {
  ast::ExpressionAST expression;
  ScopeAST scope;
};

struct ReturnAST {
  ast::ExpressionAST exp;
  template <typename T>
  ReturnAST(T &&e) //
      : exp(std::forward<T>(e)) {
  }
  ReturnAST() //
      : exp() {
  }
};

struct FunctionInvocationAST {

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

struct VariableConstructorAST { //
};

struct VariableDeclarationAST { //
};

struct WhileAST { //
};

struct DoWhileAST { //
};

} // namespace stk

#endif
