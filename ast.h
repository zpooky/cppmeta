#ifndef SP_CPP_META_AST_H
#define SP_CPP_META_AST_H

#include "entities.h"
#include <vector>
namespace ast {

enum class Incapsulation { PUBLIC, PRIVATE, PROTECTED };
struct TypeName {
  Token name;
  TypeName(Token p_name) : name(p_name) {
  }
};

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

struct InheritanceAST {
  Incapsulation incap;
  TypeName name;
  InheritanceAST(Incapsulation p_cap, TypeName p_name)
      : incap(p_cap), name(p_name) {
  }
};

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
  std::vector<InheritanceAST> inherits;

  DestructorAST dtorAST;
  /**/
  ScopeAST publicAST;
  ScopeAST privateAST;
  ScopeAST protectedAST;

  ClassAST(const Token &p_name, const std::vector<InheritanceAST> p_inherits)
      : name(p_name), inherits(p_inherits), dtorAST(), publicAST(),
        privateAST(), protectedAST() {
  }
};

struct FileAST {
  std::vector<ClassAST> m_classes;

  void push_back(const ClassAST &ast) {
    m_classes.push_back(ast);
  }
};

}

#endif
