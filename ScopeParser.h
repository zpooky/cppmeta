#ifndef SP_CPP_META_SCOPE_PARSER_H
#define SP_CPP_META_SCOPE_PARSER_H

#include "ArrayList.h"
#include "ast.h"
#include "matcher.h"

namespace ast {
// template <typename Iterator, typename AST, typename... Ts>
// match::Step<Iterator> scope(AST &result, match::Step<Iterator> start);

// template <typename Iterator, typename AST, typename T, T Headx, T... Tail>
// match::Step<Iterator> types(AST &, match::Step<Iterator>);
//
// template <typename Iterator, typename AST, typename T, T Headx>
// match::Step<Iterator> types(AST &, match::Step<Iterator>);
//
// template <typename Iterator, typename AST, typename T>
// match::Step<Iterator> single(AST &, match::Step<Iterator>);

match::Step<sp::ArrayList<Token>::const_iterator> generic_scope( //
    ScopeAST &result,                                      //
    match::Step<sp::ArrayList<Token>::const_iterator> start      //
    );
} // namespace ast

namespace ast {
// template <typename Iterator, typename AST, typename... Tail>
// match::Step<Iterator> scope(AST &result, match::Step<Iterator> start) {
//   while (start.it != start.end) {
//     auto next = types<Iterator, AST, Tail...>(result, start);
//     if (!next.valid) {
//       return start;
//     }
//     start = next;
//   }
//   return start;
// }
//
// template <typename Iterator, typename AST, typename T, T Headx, T... Tail>
// match::Step<Iterator> types(AST &result, match::Step<Iterator> start) {
//   auto next = single<Iterator, AST, T>(result, start);
//   if (next) {
//     return next;
//   }
//   return types<Iterator, AST, T, Tail...>(result, start);
// }
//
// template <typename Iterator, typename AST, typename T, T Headx>
// match::Step<Iterator> types(AST &result, match::Step<Iterator> start) {
//   return single<Iterator, AST, T>(result, start);
// }
//
// template <typename Iterator, typename AST, typename T>
// match::Step<Iterator> single(AST &result, match::Step<Iterator> start) {
//   typename T::capture_type ast;
//   match::Step<Iterator> next = start.step(ast, T{});
//   if (next.valid) {
//     result.push_back(ast);
//   }
//   return next;
// }
} // namespace ast

#endif
