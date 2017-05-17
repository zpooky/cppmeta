#include "matcher.h"
#include "lang.h"

namespace match {
bool Pattern::match(const Token &t) const {
  for (const char *keyword : cpp::keywords) {
    if (t.token == keyword) {
      return false;
    }
  }
  // TODO
  return true;
}
}
