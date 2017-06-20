#include "Pattern.h"
#include "ast.h"
#include "matcher.h"
#include "ExpressionParser.h"

namespace ast {

template <typename Iterator>
class EnumValueParser : public match::Base<EnumValueAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(EnumValueAST &capture, StepType start) const {
    Token key;
    ExpressionAST value;
    auto ret = start          //
                   .step(key) //
                   .option([&](StepType it) {
                     return it      //
                         .step("=") //
                         .step(value, ExpressionParser<Iterator>());
                   });
    return ret;
  }
};

// ex:: using ns::ns::Type;
template <typename Iterator>
class EnumParser : public match::Base<EnumAST, Iterator> {
private:
  using StepType = match::Step<Iterator>;

public:
  StepType operator()(EnumAST &capture, StepType start) const {
    Token typeName;
    TypeIdentifier underlyingType;
    auto ret =
        start
            .step("enum")                         //
            .option("class")                      //
            .step(typeName, TypeName<Iterator>()) //
            .option([&](StepType it) {
              return it                                                    //
                  .step(":")                                               //
                  .step(underlyingType, TypeIdentifierParser<Iterator>()); //
            })
            .step("{");
    if (ret) {
      std::vector<EnumValueAST> values;
      auto result = ret.repeat(values, EnumValueParser<Iterator>(), ",") //
                        .step("}")                                       //
                        .step(";");

      if (result) {
        capture = EnumAST(typeName, values);
      }
      return result;
    }
    return ret;
  }
};
}