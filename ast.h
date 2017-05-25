#ifndef SP_CPP_META_AST_H
#define SP_CPP_META_AST_H

#include "tokens.h"
#include "yaml.h"
#include <vector>

namespace ast {

// enum class Incapsulation { PUBLIC, PRIVATE, PROTECTED };

struct NamespaceAST {};

struct FunctionAST {};

struct FunctionPointerAST {};

struct OperatorAST {};

struct ConstructorAST {};

struct DestructorAST {
public:
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

struct MemberAST {};

struct ExternalAST {};

struct TypedefAST {};

/*UsingAST*/
struct UsingAST {};

struct FunctionInvocationAST {

  //
};

/*ExpressionAST*/
struct ExpressionAST {
  std::vector<Token> tokens;

  template <typename... Tail>
  ExpressionAST(Tail &&... tail) : tokens{std::forward<Token>(tail)...} {
  }
  //
};
/*TemplateAST*/
struct Typed {
  Token type;
  Token name;

  Typed() : type(), name() {
  }

  Typed(const Token &p_type, const Token &p_name) //
      : type(p_type),
        name(p_name) {
  }
};

struct Named {
  Token name;

  Named() : name() {
  }

  Named(const Token &p_name) //
      : name(p_name) {
  }
};
enum class Type { TYPED, NAMED };
struct TypenameAST {
  Typed typed;
  Named named;
  std::vector<TypenameAST> m_templates;
  Type type;

  TypenameAST(const Token &p_type, const Token &p_name) //
      : typed{p_type, p_name},
        named(),
        m_templates{},
        type(Type::TYPED) {
  }

  TypenameAST(const Token &p_name) //
      : typed(),
        named{p_name},
        m_templates(),
        type(Type::NAMED) {
  }
};

/*TypeIdentifier*/
struct TypeIdentifier {
  Token name;
  std::vector<TypenameAST> templates;

  TypeIdentifier() //
      : TypeIdentifier(Token(), {}) {
  }

  TypeIdentifier(const Token &n, const std::vector<TypenameAST> &t)
      : name(n), templates(t) {
  }
};

/*InheritanceAST*/
struct InheritanceAST {
  Token scope;
  Token virt;
  TypeIdentifier name;

  InheritanceAST(Token p_scope, Token p_virt, const TypeIdentifier &p_name)
      : scope(p_scope), virt(p_virt), name(p_name) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("scope", scope);
    result.push_back("virtual", virt);
    result.push_back("name", name.name);
    return result;
  }
};
/*StaticAST*/
struct StaticAST {
  std::vector<MemberAST> members;
  std::vector<FunctionAST> functions;
};

/*ScopeAST*/
struct ScopeAST {
  std::vector<ConstructorAST> constructors;
  std::vector<MemberAST> members;
  std::vector<OperatorAST> operators;
  std::vector<FunctionPointerAST> functionPointers;
  StaticAST staticAST;
  std::vector<TypedefAST> typdefs;
  std::vector<UsingAST> usings;

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*ClassAST*/
struct ClassAST {
  Token name;
  std::vector<InheritanceAST> inherits;
  std::vector<TypenameAST> templates;

  DestructorAST dtorAST;
  /**/
  ScopeAST publicAST;
  ScopeAST privateAST;
  ScopeAST protectedAST;

  ClassAST()
      : name(), inherits{}, templates{}, dtorAST(), //
        publicAST(), privateAST(),                  //
        protectedAST() {
  }

  ClassAST(const Token &p_name, const std::vector<InheritanceAST> &p_inherits,
           const std::vector<TypenameAST> &p_templates)
      : name(p_name),              //
        inherits(p_inherits),      //
        templates(p_templates),    //
        dtorAST(),                 //
        publicAST(), privateAST(), //
        protectedAST() {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    yaml::yaml dd;
    dd.push_back("name", name);
    dd.push_back("dtor", dtorAST);
    dd.push_back("public", publicAST);
    dd.push_back("private", privateAST);
    dd.push_back("protected", protectedAST);
    dd.push_back("inherits", yaml::List(inherits));
    result.push_back("class", dd);
    return result;
  }
};

/*FileAST*/
struct FileAST {
  std::vector<ClassAST> classes;

  void push_back(const ClassAST &ast) {
    classes.push_back(ast);
  }
};
} // namespace ast

namespace ref {}

namespace decl {
struct Class {
  /* template<typename T>
   * class Name;
   */
};
}

namespace def {}

#endif
