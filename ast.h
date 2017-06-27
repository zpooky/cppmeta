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
struct UsingTypeAST;
struct TypeIdentifier;
struct Type;
struct EnumAST;

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

// struct Named {
//   Token name;
//
//   Named() : name() {
//   }
//
//   Named(const Token &p_name) //
//       : name(p_name) {
//   }
// };

// ex:
struct TypedefAST {
  //
};
} // namespace ast

namespace pp {
/*IncludeAST*/
// ex: #include "asdads"
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

/*DefineAST*/
// ex: #define LABEL values ...
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

// ex: #ifndef key
struct IfNotDefinMacroAST {
public:
  Token key;

  IfNotDefinMacroAST() : IfNotDefinMacroAST(Token()) {
  }

  IfNotDefinMacroAST(const Token &p_key) : key(p_key) {
  }
};
} // namespace pp

namespace tmp {
// ex: sp::ast::Type<t1,sp::t2>
struct TypeArgumentAST {
  Token named;
  std::vector<ast::NamespaceAST> namespaces;
  std::vector<TypeArgumentAST> typeArguments;

  TypeArgumentAST() //
      : named(),
        namespaces(),
        typeArguments() {
  }

  TypeArgumentAST(const Token &n, const std::vector<ast::NamespaceAST> &nss,
                  const std::vector<TypeArgumentAST> &ta) //
      : named(n),
        namespaces(nss),
        typeArguments(ta) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("named", named);
    result.push_back("namespaces", yaml::List(namespaces));
    result.push_back("typeArguments", yaml::List(typeArguments));
    return result;
  }
};

enum class Type { TYPED, NAMED };
// ex: Name<asd,asdasd<asdsdd,asdsad>>
struct TemplateTypenameAST {
  Token typed;
  Token named;
  std::vector<TemplateTypenameAST> templates;
  Type type;

  // ex: <size_t i>
  TemplateTypenameAST(const Token &p_type, const Token &p_name) //
      : typed(p_type),
        named(p_name),
        templates{},
        type(Type::TYPED) {
  }

  // ex: <Token>
  TemplateTypenameAST(const Token &p_name) //
      : typed(),
        named{p_name},
        templates(),
        type(Type::NAMED) {
  }

  TemplateTypenameAST() //
      : typed(),
        named(),
        templates(),
        type(Type::NAMED) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("typed", typed);
    result.push_back("named", named);
    result.push_back("templates", yaml::List(templates));
    result.push_back("type",
                     type == Type::TYPED ? String("TYPED") : String("NAMED"));
    return result;
  }
};
} // namespace tmp

namespace ast {
/*TypeIdentifier*/
// ex: sp::Type<T,int>
struct TypeIdentifier {
  Token name;
  std::vector<TypeIdentifier> typeArguments;
  std::vector<Token> namespaces;

  TypeIdentifier() //
      : TypeIdentifier(Token(), {}, {}) {
  }

  TypeIdentifier(const Token &n, const std::vector<TypeIdentifier> &t,
                 const std::vector<Token> nss)
      : name(n), typeArguments(t), namespaces(nss) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("name", name);
    result.push_back("templates", yaml::List(typeArguments));
    result.push_back("namespaces", yaml::List(namespaces));
    return result;
  }
};

struct ParameterTypeAST {
  std::vector<Token> qualifiers;
  TypeIdentifier type;
  std::vector<Token> refs;
  std::vector<Token> ptrs;

  ParameterTypeAST() //
      : qualifiers(),
        type(),
        refs(),
        ptrs() {
  }

  ParameterTypeAST(const std::vector<Token> q, const TypeIdentifier &t,
                   const std::vector<Token> &r, const std::vector<Token> &p) //
      : qualifiers(q),
        type(t),
        refs(r),
        ptrs(p) {
  }

  //
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*TypeExpressionAST*/
struct TypeExpressionAST {
  std::vector<Token> tokens;

  template <typename... Tail>
  TypeExpressionAST(Tail &&... tail) //
      : tokens{std::forward<Token>(tail)...} {
  }
  //
};

/*ExpressionAST*/
struct ExpressionAST {
  std::vector<Token> tokens;

  template <typename... Tail>
  ExpressionAST(Tail &&... tail) //
      : tokens{std::forward<Token>(tail)...} {
  }
  //
};

struct ParameterAST {
  ParameterTypeAST type;
  Token name;
  ExpressionAST defaultValue;

  ParameterAST() //
      : type(),
        name(),
        defaultValue() {
  }
  ParameterAST(const ParameterTypeAST &t, const Token &n,
               const ExpressionAST &dv) //
      : type(t),
        name(n),
        defaultValue(dv) {
  }
  //
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*FunctionDefinitionAST*/
struct FunctionDeclarationAST {
  std::vector<tmp::TemplateTypenameAST> templates;
  std::vector<Token> prefix;
  ParameterTypeAST returnType;
  Token functionName;
  std::vector<ParameterAST> parameters;
  std::vector<Token> postfix;
  bool pureVirtual;
  bool deleted;
  //
  FunctionDeclarationAST()
      : //
        templates(),
        prefix(), returnType(), functionName(), parameters(), postfix(),
        pureVirtual(false), deleted(false) {
  }

  FunctionDeclarationAST(const std::vector<tmp::TemplateTypenameAST> &t,
                         const std::vector<Token> &pre,
                         const ParameterTypeAST &ret, const Token &n,
                         const std::vector<ParameterAST> &params,
                         const std::vector<Token> &post, bool pv, bool dd)
      : //
        templates(t),
        prefix(pre), returnType(ret), functionName(n), parameters(params),
        postfix(post), pureVirtual(pv), deleted(dd) {
  }
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*FunctionDefinitionAST*/
struct FunctionDefinitionAST {
  ParameterTypeAST returnType;
  Token functionName;
  std::vector<ParameterAST> parameters;

  FunctionDefinitionAST() //
      : returnType(),
        functionName(),
        parameters() {
  }

  FunctionDefinitionAST(const ParameterTypeAST &r, const Token n,
                        const std::vector<ParameterAST> &p) //
      : returnType(r),
        functionName(n),
        parameters(p) {
  }
  //
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

/*OperatorDefintionAST*/
struct OperatorDefinitionAST { //
};

/*OperatorDeclarationAST*/
struct OperatorDeclarationAST { //
};

/*CtorDeclarationAST*/
struct CtorDeclarationAST { //
};

/*CtorDefintionAST*/
struct CtorDefinitionAST { //
};

/*DtorDeclarationAST*/
struct DtorDeclarationAST { //
};

/*DtorDefintionAST*/
struct DtorDefinitionAST { //
};

// ex: namespace key { ... }
struct NamespaceAST {
  Token key;
  // namespace
  std::vector<NamespaceAST> namespaces;
  // type
  std::vector<ClassAST> classes;
  std::vector<EnumAST> enums;
  // using
  std::vector<UsingNamespaceAST> usingNamespaces;
  std::vector<UsingAliasAST> usingAlias;
  std::vector<UsingTypeAST> usingType;
  //
  std::vector<FunctionDefinitionAST> funtionDefinitions;
  std::vector<FunctionDeclarationAST> functionDeclarations;

  NamespaceAST() : key() {
  }
  NamespaceAST(const Token &p_key) : key(p_key) {
  }

  /*namespace*/
  void push_back(const NamespaceAST &ast) {
    namespaces.push_back(ast);
  }

  /*class*/
  void push_back(const ClassAST &ast) {
    classes.push_back(ast);
  }

  /*enum*/
  void push_back(const EnumAST &ast) {
    enums.push_back(ast);
  }

  /*Using*/
  void push_back(const UsingNamespaceAST &ast) {
    usingNamespaces.push_back(ast);
  }

  void push_back(const UsingAliasAST &ast) {
    usingAlias.push_back(ast);
  }

  void push_back(const UsingTypeAST &ast) {
    usingType.push_back(ast);
  }

  /*function*/
  void push_back(const FunctionDefinitionAST &ast) {
    funtionDefinitions.push_back(ast);
  }

  void push_back(const FunctionDeclarationAST &ast) {
    functionDeclarations.push_back(ast);
  }

  /*operator*/
  void push_back(const OperatorDefinitionAST &ast) {
    // TODO
  }

  void push_back(const OperatorDeclarationAST &ast) {
    // TODO
  }
  /*ctor*/
  void push_back(const CtorDefinitionAST &ast) {
    // TODO
  }

  void push_back(const CtorDeclarationAST &ast) {
    // TODO
  }
  /*dtor*/
  void push_back(const DtorDefinitionAST &ast) {
    // TODO
  }

  void push_back(const DtorDeclarationAST &ast) {
    // TODO
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

struct FunctionPointerAST {
  ParameterTypeAST returnType;
  Token ref;
  Token name;
  std::vector<ParameterAST> paramters;
  //
  FunctionPointerAST()
      : //
        returnType(),
        ref(), name(), paramters() {
  }

  FunctionPointerAST(const ParameterTypeAST &rt, const Token &r, const Token &n,
                     const std::vector<ParameterAST> &p)
      : //
        returnType(rt),
        ref(r), name(n), paramters(p) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

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

/*UsingNamespaceAST*/
// ex: using namespace std
struct UsingNamespaceAST {
  std::vector<Token> namespaces;

  UsingNamespaceAST() //
      : namespaces() {
  }

  UsingNamespaceAST(const std::vector<Token> &n) //
      : namespaces(n) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

struct FunctionInvocationAST {

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

struct EnumValueAST {
  Token key;
  ExpressionAST value;

  EnumValueAST()
      : //
        key(),
        value() {
  }

  EnumValueAST(const Token &k, const ExpressionAST &v) //
      : key(k),
        value(v) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

// enum class Incapsulation { PUBLIC, PRIVATE, PROTECTED };
struct EnumAST {
  Token typeName;
  std::vector<EnumValueAST> values;

  EnumAST()
      : //
        typeName(),
        values() {
  }

  EnumAST(const Token &n, const std::vector<EnumValueAST> &v) //
      : typeName(n),
        values(v) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("name", typeName);
    result.push_back("values", yaml::List(values));
    return result;
  }
};

/*UsingAliasAST*/
// ex: using alias = concrete
struct UsingAliasAST {
  std::vector<tmp::TemplateTypenameAST> templates;
  TypeIdentifier alias;
  TypeIdentifier concrete;

  UsingAliasAST() : templates(), alias(), concrete() {
  }

  UsingAliasAST(const std::vector<tmp::TemplateTypenameAST> &t,
                const TypeIdentifier &a, const TypeIdentifier &c)
      : templates(t), alias(a), concrete(c) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("templates", yaml::List(templates));
    result.push_back("alias", alias);
    result.push_back("concrete", concrete);
    return result;
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

/*UsingType*/
// ex: using std::String
struct UsingTypeAST {
  TypeIdentifier alias;

  UsingTypeAST() : alias() {
  }

  explicit UsingTypeAST(const TypeIdentifier &a) : alias(a) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("alias", alias);
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
// class Typename : public T<int>,private asd { ... }
struct ClassAST {
  Token name;
  std::vector<InheritanceAST> inherits;
  std::vector<tmp::TemplateTypenameAST> templates;

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
           const std::vector<tmp::TemplateTypenameAST> &p_templates)
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
    dd.push_back("template", yaml::List(templates));
    result.push_back("class", dd);
    return result;
  }
};

// ex: const T (&x)[N]
struct TemplateCArrayAST {
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

} // namespace ast

namespace ast {
/*FileAST*/
struct FileAST {
  std::vector<ClassAST> classes;
  std::vector<EnumAST> enums;
  // preprocess
  std::vector<pp::IncludeAST> includes;
  std::vector<pp::DefineAST> defines;
  std::vector<pp::IfNotDefinMacroAST> ifNotDefines;
  // using
  std::vector<UsingNamespaceAST> usingNamespaces;
  std::vector<UsingAliasAST> usingAlias;
  std::vector<UsingTypeAST> usingType;
  // namespace
  std::vector<NamespaceAST> namespaces;
  //
  std::vector<FunctionDefinitionAST> funtionDefinitions;
  std::vector<FunctionDeclarationAST> functionDeclarations;

  /*ClassAST*/
  void push_back(const ClassAST &ast) {
    classes.push_back(ast);
  }

  /*Enum*/
  void push_back(const EnumAST &ast) {
    enums.push_back(ast);
  }

  /*preprocessor*/
  void push_back(const pp::IncludeAST &ast) {
    includes.push_back(ast);
  }

  void push_back(const pp::DefineAST &ast) {
    defines.push_back(ast);
  }

  void push_back(const pp::IfNotDefinMacroAST &ast) {
    ifNotDefines.push_back(ast);
  }

  /*using*/
  void push_back(const UsingNamespaceAST &ast) {
    usingNamespaces.push_back(ast);
  }

  void push_back(const UsingAliasAST &ast) {
    usingAlias.push_back(ast);
  }

  void push_back(const UsingTypeAST &ast) {
    usingType.push_back(ast);
  }

  /*namespace*/
  void push_back(const NamespaceAST &ast) {
    namespaces.push_back(ast);
  }

  /*Function*/
  void push_back(const FunctionDefinitionAST &ast) {
    funtionDefinitions.push_back(ast);
  }

  void push_back(const FunctionDeclarationAST &ast) {
    functionDeclarations.push_back(ast);
  }
  /*operator*/
  void push_back(const OperatorDefinitionAST &ast) {
    // TODO
  }

  void push_back(const OperatorDeclarationAST &ast) {
    // TODO
  }
  /*ctor*/
  void push_back(const CtorDefinitionAST &ast) {
    // TODO
  }

  void push_back(const CtorDeclarationAST &ast) {
    // TODO
  }
  /*dtor*/
  void push_back(const DtorDefinitionAST &ast) {
    // TODO
  }

  void push_back(const DtorDeclarationAST &ast) {
    // TODO
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
