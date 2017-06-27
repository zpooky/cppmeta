#ifndef SP_CPP_META_SCOPE_PARSER_H
#define SP_CPP_META_SCOPE_PARSER_H
#include "EnumParser.h"
#include "ScopeParser.h"
#include "parser.h"

#define GENERIC_SCOPE(RESULT, AST, STEP)                                       \
  scope<Iterator, RESULT, ClassParser<Iterator>, EnumParser<Iterator>>(AST,    \
                                                                       STEP)

namespace ast {
// template <typename Iterator, typename AST, typename... Ts>
// match::Step<Iterator> scope(AST &result, match::Step<Iterator> start);

template <typename Iterator, typename AST, typename T, T Headx, T... Tail>
match::Step<Iterator> types(AST &, match::Step<Iterator>);

template <typename Iterator, typename AST, typename T, T Headx>
match::Step<Iterator> types(AST &, match::Step<Iterator>);

template <typename Iterator, typename AST, typename T>
match::Step<Iterator> single(AST &, match::Step<Iterator>);

} // namespace ast

namespace ast {
template <typename Iterator, typename AST, typename... Tail>
match::Step<Iterator> scope(AST &result, match::Step<Iterator> start) {
  while (start.it != start.end) {
    auto next = types<Iterator, AST, Tail...>(result, start);
    if (!next.valid) {
      return start;
    }
    start = next;
  }
  return start;
}

template <typename Iterator, typename AST, typename T, T Headx, T... Tail>
match::Step<Iterator> types(AST &result, match::Step<Iterator> start) {
  auto next = single<Iterator, AST, T>(result, start);
  if (next) {
    return next;
  }
  return types<Iterator, AST, T, Tail...>(result, start);
}

template <typename Iterator, typename AST, typename T, T Headx>
match::Step<Iterator> types(AST &result, match::Step<Iterator> start) {
  return single<Iterator, AST, T>(result, start);
}

template <typename Iterator, typename AST, typename T>
match::Step<Iterator> single(AST &result, match::Step<Iterator> start) {
  typename T::capture_type ast;
  match::Step<Iterator> next = start.step(ast, T{});
  if (next.valid) {
    result.push_back(ast);
  }
  return next;
}

// template <typename Iterator, typename AST>
// match::Step<Iterator> generic_scope(AST &result, match::Step<Iterator> start)
// {
//   while (start.it != start.end) {
//     {
//       ast::ClassAST ast;
//       auto next = start.step(ast, ast::ClassParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::EnumAST ast;
//       auto next = start.step(ast, ast::EnumParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::NamespaceAST ast;
//       auto next = start.step(ast, ast::NamespaceParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::UsingNamespaceAST ast;
//       auto next = start.step(ast, ast::UsingNamespaceParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::UsingAliasAST ast;
//       auto next = start.step(ast, ast::UsingAliasParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::UsingTypeAST ast;
//       auto next = start.step(ast, ast::UsingTypeParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::FunctionDefinitionAST ast;
//       auto next = start.step(ast, ast::FunctionDefinitionParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     {
//       ast::FunctionDeclarationAST ast;
//       auto next = start.step(ast,
//       ast::FunctionDeclarationParser<Iterator>());
//       if (next.valid) {
//         result.push_back(ast);
//         start = next;
//         continue;
//       }
//     }
//     return start;
//   }
//   return start;
// }
} // namespace ast

#endif
