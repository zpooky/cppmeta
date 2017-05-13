#ifndef SP_CPP_META_AST_H
#define SP_CPP_META_AST_H

#include "entities.h"
#include <vector>
#include "yaml.h"

namespace ast {

// enum class Incapsulation { PUBLIC, PRIVATE, PROTECTED };
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
  Token scope;
  Token virt;
  TypeName name;

  InheritanceAST(Token p_scope, Token p_virt, TypeName p_name)
      : scope(p_scope), virt(p_virt), name(p_name) {
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

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("class", name);
    return result;
  }
};

struct FileAST {
  std::vector<ClassAST> classes;

  void push_back(const ClassAST &ast) {
    classes.push_back(ast);
  }
};
}

#endif
