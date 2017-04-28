#ifndef SP_CPP_META_TOKENIZER_H
#define SP_CPP_META_TOKENIZER_H

#include "entities.h"
#include <vector>

class Tokenizer {
private:
  File file;

  bool is_blank(char c) const;
  bool is_newline(char c) const;


public:
  Tokenizer(const File &p_file) : file(p_file) {
  }
  std::vector<Token> tokenize();
};

#endif
