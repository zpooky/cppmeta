#ifndef SP_CPP_META_TOKENIZER_H
#define SP_CPP_META_TOKENIZER_H

#include "entities.h"
#include <vector>

class Tokenizer {
private:
  File file;

public:
  Tokenizer(const File &p_file) : file(p_file) {
  }

  std::vector<Token> tokenize();
};

class StringTokenizer {
public:
  template <typename Iterator>
  Iterator parse(Iterator it, Iterator end, std::string &result) {
    bool c = true;
    while (it != end && c) {
      char datum = *(it)++;
      if (datum == '"') {
        if (!result.empty() && result.back() != '\\') {
          c = false;
        }
      }
      result.push_back(datum);
    }
    return it;
  }
};

#endif
