#ifndef SP_CPP_META_STRIP_COMMENTS_H
#define SP_CPP_META_STRIP_COMMENTS_H

#include "entities.h"
#include <vector>

class StripComments {
public:
  std::vector<Token> parse(const std::vector<Token> &);
};

#endif
