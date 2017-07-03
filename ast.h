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
struct ParameterAST;
struct ParameterTypeAST;

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

  ParameterTypeAST(const ParameterTypeAST &o) //
      : qualifiers(o.qualifiers),
        type(o.type),
        refs(o.refs),
        ptrs(o.ptrs) {
  }

  ~ParameterTypeAST() {
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

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("exp", yaml::List(tokens));
    return result;
  }
};

struct FunctionPointerAST {
  ParameterTypeAST returnType;
  Token ref;
  Token name;
  std::vector<ParameterAST> paramters;
  //
  FunctionPointerAST() //
      : returnType(),
        ref(),
        name(),
        paramters() {
  }

  FunctionPointerAST(const ParameterTypeAST &rt, const Token &r, const Token &n,
                     const std::vector<ParameterAST> &p)
      //
      : returnType(rt),
        ref(r),
        name(n),
        paramters(p) {
  }
  FunctionPointerAST(const FunctionPointerAST &o) //
      : returnType(o.returnType),
        ref(o.ref),
        name(o.name),
        paramters(o.paramters) {
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("return", returnType);
    result.push_back("ref", ref);
    result.push_back("name", name);
    result.push_back("parameters", yaml::List(paramters));
    return result;
  }
};

/*TemplateCArrayAST*/
// ex: const T (&x)[N]
struct TemplateCArrayAST {
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    return result;
  }
};

enum class ParamCat { NONE, PARAMETER, FUNC_POINTER, TEMPLATE_ARRAY };
/*ParameterEither*/
struct ParameterEither {
private:
  template <typename T>
  using TheParamType =
      typename std::aligned_storage<sizeof(T), alignof(T)>::type;

  union PE {
    int dummy;
    TheParamType<ParameterTypeAST> type;
    TheParamType<FunctionPointerAST> fp;
    TheParamType<TemplateCArrayAST> carr;

    // alignas(alignof(ParameterTypeAST)) uint8_t
    // type[sizeof(ParameterTypeAST)];
    // alignas(alignof(FunctionPointerAST)) uint8_t
    // fp[sizeof(FunctionPointerAST)];
    // alignas(alignof(TemplateCArrayAST)) uint8_t
    // carr[sizeof(TemplateCArrayAST)];
  };

public:
  PE either;
  ParamCat pc;

  ParameterEither() //
      : pc(ParamCat::NONE) {
    new (&either.dummy) int();
  }

  ParameterEither(const ParameterTypeAST &pt) //
      : pc(ParamCat::PARAMETER) {
    new (&either.type) ParameterTypeAST(pt);
  }

  ParameterEither(const FunctionPointerAST &pfp) //
      : pc(ParamCat::FUNC_POINTER) {
    new (&either.fp) FunctionPointerAST(pfp);
  }

  ParameterEither(const TemplateCArrayAST &pcarr) //
      : pc(ParamCat::TEMPLATE_ARRAY) {
    new (&either.carr) TemplateCArrayAST(pcarr);
  }

  ParameterEither(const ParameterEither &o) //
      : pc(ParamCat::NONE) {
    operator=(o);
  }

  ParameterEither(ParameterEither &&o) //
      : pc(ParamCat::NONE) {
    operator=(std::move(o));
  }

  ParameterEither &operator=(const ParameterEither &o) {
    destruct();
    pc = o.pc;
    if (pc == ParamCat::PARAMETER) {
      const auto &other =
          *reinterpret_cast<const ParameterTypeAST *>(&o.either.type);

      new (&either.type) ParameterTypeAST(other);
    } else if (pc == ParamCat::FUNC_POINTER) {
      const auto &other =
          *reinterpret_cast<const FunctionPointerAST *>(&o.either.fp);

      new (&either.fp) FunctionPointerAST(other);
    } else if (pc == ParamCat::TEMPLATE_ARRAY) {
      const auto &other =
          *reinterpret_cast<const TemplateCArrayAST *>(&o.either.carr);

      new (&either.carr) TemplateCArrayAST(other);
    }
    return *this;
  }

  ParameterEither &operator=(ParameterEither &&o) {
    destruct();
    pc = o.pc;
    if (pc == ParamCat::PARAMETER) {
      auto &other = *reinterpret_cast<ParameterTypeAST *>(&o.either.type);

      new (&either.type) ParameterTypeAST(std::move(other));
    } else if (pc == ParamCat::FUNC_POINTER) {
      auto &other = *reinterpret_cast<FunctionPointerAST *>(&o.either.fp);

      new (&either.fp) FunctionPointerAST(std::move(other));
    } else if (pc == ParamCat::TEMPLATE_ARRAY) {
      auto &other = *reinterpret_cast<TemplateCArrayAST *>(&o.either.carr);

      new (&either.carr) TemplateCArrayAST(std::move(other));
    }
    return *this;
  }

  ~ParameterEither() {
    destruct();
  }

  void destruct() {
    if (pc == ParamCat::PARAMETER) {
      auto &ptr = *reinterpret_cast<ParameterTypeAST *>(&either.type);
      ptr.~ParameterTypeAST();
    } else if (pc == ParamCat::FUNC_POINTER) {
      auto &ptr = *reinterpret_cast<FunctionPointerAST *>(&either.fp);
      ptr.~FunctionPointerAST();
    } else if (pc == ParamCat::TEMPLATE_ARRAY) {
      auto &ptr = *reinterpret_cast<TemplateCArrayAST *>(&either.carr);
      ptr.~TemplateCArrayAST();
    }
    pc = ParamCat::NONE;
  }

  yaml::yaml to_yaml() const {
    yaml::yaml result;
    if (pc == ParamCat::PARAMETER) {
      auto &ptr = *reinterpret_cast<const ParameterTypeAST *>(&either.type);
      return ptr.to_yaml();
    } else if (pc == ParamCat::FUNC_POINTER) {
      auto &ptr = *reinterpret_cast<const FunctionPointerAST *>(&either.fp);
      return ptr.to_yaml();
    } else if (pc == ParamCat::TEMPLATE_ARRAY) {
      auto &ptr = *reinterpret_cast<const TemplateCArrayAST *>(&either.carr);
      return ptr.to_yaml();
    }
    return result;
  }
};

struct ReturnTypeAST {
  ParameterEither type;
  ReturnTypeAST() //
      : type() {
  }

  template <typename T>
  ReturnTypeAST(T &&arg) //
      : type(std::forward<T>(arg)) {
  }

  yaml::yaml to_yaml() const {
    return type.to_yaml();
  }
};

/*ParamterAST*/
struct ParameterAST {
  ParameterEither type;
  Token name;
  ExpressionAST defaultValue;

  ParameterAST() //
      : type(),
        name(),
        defaultValue() {
  }

  ParameterAST &operator=(const ParameterAST &o) {
    type = o.type;
    name = o.name;
    defaultValue = o.defaultValue;
    return *this;
  }

  template <typename T>
  ParameterAST(T &&o, const Token &n,
               const ExpressionAST &dv) //
      : type(std::forward<T>(o)),
        name(n),
        defaultValue(dv) {
  }

  ~ParameterAST() {
  }

  //
  yaml::yaml to_yaml() const {
    yaml::yaml result;
    result.push_back("type", type);
    result.push_back("name", name);
    result.push_back("default", defaultValue);
    return result;
  }
};

/*FunctionDefinitionAST*/
struct FunctionDefinitionAST {
  std::vector<tmp::TemplateTypenameAST> templates;
  std::vector<Token> prefix;
  ParameterTypeAST returnType;
  Token functionName;
  std::vector<ParameterAST> parameters;
  std::vector<Token> postfix;
  bool pureVirtual;
  bool deleted;
  //
  FunctionDefinitionAST()
      : //
        templates(),
        prefix(), returnType(), functionName(), parameters(), postfix(),
        pureVirtual(false), deleted(false) {
  }

  FunctionDefinitionAST(const std::vector<tmp::TemplateTypenameAST> &t,
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
    result.push_back("template", yaml::List(templates));
    result.push_back("prefix", yaml::List(prefix));

    result.push_back("return", returnType);
    result.push_back("name", functionName);

    result.push_back("parameters", yaml::List(parameters));
    result.push_back("postfix", yaml::List(postfix));
    // result.push_back("pure-virtual", pureVirtual);
    // result.push_back("deleted", deleted);
    return result;
  }
};

/*FunctionDefinitionAST*/
struct FunctionDeclarationAST {
  ParameterTypeAST returnType;
  Token functionName;
  std::vector<ParameterAST> parameters;

  FunctionDeclarationAST() //
      : returnType(),
        functionName(),
        parameters() {
  }

  FunctionDeclarationAST(const ParameterTypeAST &r, const Token n,
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
// #<{(|StaticAST|)}>#
// struct StaticAST {
// };

/*MemberDefinitionAST*/
struct MemberDefinitionAST {
  ParameterEither type;
  Token name;
  MemberDefinitionAST() //
      : type(),
        name() {
  }

  MemberDefinitionAST(const ParameterEither &t, const Token &n) //
      : type(t),
        name(n) {
  }
};


/*ScopeAST*/
struct ScopeAST {
  // types
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
  // functions
  std::vector<FunctionDefinitionAST> funtionDefinitions;
  std::vector<FunctionDeclarationAST> functionDeclarations;
  // operators
  std::vector<OperatorDefinitionAST> operatorDefinitions;
  std::vector<OperatorDeclarationAST> operatorDeclarations;
  // ctor
  std::vector<CtorDefinitionAST> ctorDefinitions;
  std::vector<CtorDeclarationAST> ctorDeclarations;
  // dtor
  std::vector<DtorDefinitionAST> dtorDefinitions;
  std::vector<DtorDeclarationAST> dtorDeclarations;
  // member
  std::vector<MemberDefinitionAST> memberDefinitions;

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
    operatorDefinitions.push_back(ast);
  }

  void push_back(const OperatorDeclarationAST &ast) {
    operatorDeclarations.push_back(ast);
  }
  /*ctor*/
  void push_back(const CtorDefinitionAST &ast) {
    ctorDefinitions.push_back(ast);
  }

  void push_back(const CtorDeclarationAST &ast) {
    ctorDeclarations.push_back(ast);
  }
  /*dtor*/
  void push_back(const DtorDefinitionAST &ast) {
    dtorDefinitions.push_back(ast);
  }

  void push_back(const DtorDeclarationAST &ast) {
    dtorDeclarations.push_back(ast);
  }
  /*MemberDefinition*/
  void push_back(const MemberDefinitionAST &ast) {
    memberDefinitions.push_back(ast);
  }

  virtual yaml::yaml to_yaml() const {
    yaml::yaml result;
    // TODO
    result.push_back("includes", yaml::List(includes));
    result.push_back("defines", yaml::List(defines));
    result.push_back("classes", yaml::List(classes));
    result.push_back("using-namespace", yaml::List(usingNamespaces));
    result.push_back("using-alias", yaml::List(usingAlias));
    result.push_back("namespace", yaml::List(classes));
    result.push_back("function-declaration", yaml::List(functionDeclarations));
    return result;
  }
};

// ex: namespace key { ... }
struct NamespaceAST : ScopeAST {
  Token key;

  NamespaceAST() //
      : key() {
  }
  NamespaceAST(const Token &p_key) //
      : key(p_key) {
  }

  // TODO
  // yaml::yaml to_yaml() const {
  //   yaml::yaml result(ScopeAST.to_yaml());
  //   result.push_back("namespace", key);
  //   return result;
  // }
};

/*ClassAST*/
// ex:
// template<int = 1, T>
// class Typename : public T<int>,private asd { ... }
struct ClassAST : ScopeAST {
  Token name;
  std::vector<InheritanceAST> inherits;
  std::vector<tmp::TemplateTypenameAST> templates;

  /**/
  ScopeAST publicAST;
  ScopeAST privateAST;
  ScopeAST protectedAST;

  ClassAST()
      : name(),       //
        inherits{},   //
        templates{},  //
        publicAST(),  //
        privateAST(), //
        protectedAST() {
  }

  ClassAST(const Token &p_name, const std::vector<InheritanceAST> &p_inherits,
           const std::vector<tmp::TemplateTypenameAST> &p_templates)
      : name(p_name),              //
        inherits(p_inherits),      //
        templates(p_templates),    //
        publicAST(), privateAST(), //
        protectedAST() {
  }

  yaml::yaml to_yaml() const {
    // TODO super.to_yaml()
    yaml::yaml result;
    yaml::yaml dd;
    dd.push_back("name", name);
    dd.push_back("public", publicAST);
    dd.push_back("private", privateAST);
    dd.push_back("protected", protectedAST);
    dd.push_back("inherits", yaml::List(inherits));
    dd.push_back("template", yaml::List(templates));
    result.push_back("class", dd);
    return result;
  }
};

} // namespace ast

namespace ast {
/*FileAST*/
struct FileAST : ScopeAST { //
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
