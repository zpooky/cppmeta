#ifndef SP_CPP_META_AST_H
#define SP_CPP_META_AST_H

#include "tokens.h"
#include "yaml.h"
#include <vector>

namespace ast {
struct ClassAST;
struct NamespaceAST;
struct UsingNamespaceAST;
struct UsingAliasAST;

// enum class Incapsulation { PUBLIC, PRIVATE, PROTECTED };

struct NamespaceAST {
  Token key;
  std::vector<NamespaceAST> namespaces;
  std::vector<ClassAST> classes;
  std::vector<UsingNamespaceAST> usingNamespaces;
  std::vector<UsingAliasAST> usingAlias;

  NamespaceAST() : key() {
  }
  NamespaceAST(const Token &p_key) : key(p_key) {
  }

  void push_back(const NamespaceAST &ast) {
    namespaces.push_back(ast);
  }

  void push_back(const ClassAST &ast) {
    classes.push_back(ast);
  }

  void push_back(const UsingNamespaceAST &ast) {
    usingNamespaces.push_back(ast);
  }

  void push_back(const UsingAliasAST &ast) {
    usingAlias.push_back(ast);
  }

  yaml::yaml to_yaml() const {
    yaml::yaml n;
    n.push_back("classes", yaml::List(classes));
    n.push_back("namespaces", yaml::List(namespaces));
    n.push_back("using-namespace", yaml::List(usingNamespaces));
    n.push_back("using-alias", yaml::List(usingAlias));

    yaml::yaml yaml;
    yaml.push_back("token", key);
    yaml.push_back("namespace", n);
    return yaml;
  }
};

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
/*DefineAST*/

struct DefineAST {
  Token key;
  std::vector<Token> values;

  DefineAST(const Token &p_key, const std::vector<Token> &p_values)
      : key(p_key), values(p_values) {
  }

  DefineAST() : key(), values() {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("key", key);
    return result;
  }
};

struct IfNotDefinMacroAST {
public:
  Token key;

  IfNotDefinMacroAST() : IfNotDefinMacroAST(Token()) {
  }

  IfNotDefinMacroAST(const Token &p_key) : key(p_key) {
  }
};

/*UsingAliasAST*/
// ex: using alias = concrete
struct UsingAliasAST {

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*UsingNamespaceAST*/
// ex: using namespace std
struct UsingNamespaceAST {

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};
/*UsingType*/
//ex: using std::String
struct UsingTypeAST {
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

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
namespace tmp {
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
}

/*TypeIdentifier*/
// ex: Typename<T,int>
struct TypeIdentifier {
  Token name;
  std::vector<tmp::TypenameAST> templates;
  std::vector<Token> namespaces;

  TypeIdentifier() //
      : TypeIdentifier(Token(), {}, {}) {
  }

  TypeIdentifier(const Token &n, const std::vector<tmp::TypenameAST> &t,
                 const std::vector<Token> nss)
      : name(n), templates(t), namespaces(nss) {
  }
};

/*InheritanceAST*/
// ex: public virtual Typename<int>
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
  std::vector<UsingNamespaceAST> usingNamespaces;
  std::vector<UsingAliasAST> usingAlias;

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*ClassAST*/
// ex:
// template<int = 1, T>
// class Typename : public T<int>,private asd {
//...
struct ClassAST {
  Token name;
  std::vector<InheritanceAST> inherits;
  std::vector<tmp::TypenameAST> templates;

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
           const std::vector<tmp::TypenameAST> &p_templates)
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
/*IncludeAST*/

struct IncludeAST {
  Token file;
  IncludeAST(const Token &p_file) : file(p_file) {
  }
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("file", file);
    return result;
  }
};

/*FileAST*/
struct FileAST {
  std::vector<ClassAST> classes;
  // preprocess
  std::vector<IncludeAST> includes;
  std::vector<DefineAST> defines;
  std::vector<IfNotDefinMacroAST> ifNotDefines;
  std::vector<UsingNamespaceAST> usingNamespaces;
  std::vector<UsingAliasAST> usingAlias;
  std::vector<NamespaceAST> namespaces;

  void push_back(const IncludeAST &ast) {
    includes.push_back(ast);
  }

  void push_back(const ClassAST &ast) {
    classes.push_back(ast);
  }

  void push_back(const DefineAST &ast) {
    defines.push_back(ast);
  }

  void push_back(const IfNotDefinMacroAST &ast) {
    ifNotDefines.push_back(ast);
  }

  void push_back(const UsingNamespaceAST &ast) {
    usingNamespaces.push_back(ast);
  }

  void push_back(const UsingAliasAST &ast) {
    usingAlias.push_back(ast);
  }

  void push_back(const NamespaceAST &ast) {
    namespaces.push_back(ast);
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("includes", yaml::List(includes));
    result.push_back("defines", yaml::List(defines));
    result.push_back("classes", yaml::List(classes));
    result.push_back("using-namespace", yaml::List(usingNamespaces));
    result.push_back("using-alias", yaml::List(usingAlias));
    result.push_back("namespace", yaml::List(classes));
    return result;
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
