#ifndef SP_CPP_META_AST_H
#define SP_CPP_META_AST_H

#include "entities.h"
#include <vector>

struct NamespaceAST {};

struct FunctionAST {};

struct FunctionPointerAST {};

struct OperatorAST {};

struct ConstructorAST {};

struct DestructorAST {};

struct MemberAST {};

struct ExternalAST {};

struct TypedefAST {};

struct UsingAST {};

struct StaticAST {
  std::vector<MemberAST> members;
  std::vector<FunctionAST> functions;
};

struct ScopeAST {
  std::vector<ConstructorAST> constructors;
  std::vector<MemberAST> members;
  std::vector<OperatorAST> operators;
  std::vector<FunctionPointerAST> functionPointers;
  StaticAST staticAST;
  std::vector<TypedefAST> typdefs;
  std::vector<UsingAST> usings;
};

struct ClassAST {
  Token name;
  DestructorAST dtorAST;
  /**/
  ScopeAST publicAST;
  ScopeAST privateAST;
  ScopeAST protectedAST;

  ClassAST(const Token &p_name)
      : name(p_name), dtorAST(), publicAST(), privateAST(), protectedAST() {
  }
};

struct FileAST {
  std::vector<ClassAST> m_classes;

  void push_back(const ClassAST &ast) {
    m_classes.push_back(ast);
  }
};

#endif
