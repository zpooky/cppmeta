#ifndef SP_CPP_META_AST_H
#define SP_CPP_META_AST_H

#include <vector>

struct FunctionAST {
};

struct FunctionPointerAST {
};

class OperatorAST {
};

struct ConstructorAST {
};

struct DestructorAST {
};

struct MemberAST {
};

struct ExternalAST {
};

struct TypedefAST {
};

struct UsingAST {
};

struct StaticAST{
  std::vector<MemberAST> members;
  std::vector<FunctionAST> functions;
};

class ScopeAST {
  std::vector<ConstructorAST> constructors;
  std::vector<MemberAST> members;
  std::vector<OperatorAST> operators;
  std::vector<FunctionPointerAST> functionPointers;
  StaticAST staticAST;
  std::vector<TypedefAST> typdefs;
  std::vector<UsingAST> usings;

};

struct StructAST {
};

struct ClassAST {
  DestructorAST dtorAST;
  /**/
  ScopeAST publicAST;
  ScopeAST privateAST;
  ScopeAST protectedAST;
};

struct FileAST {
  std::vector<ClassAST> m_classes;

  void push_back(const ClassAST&ast){
    m_classes.push_back(ast);
  }
};

#endif
