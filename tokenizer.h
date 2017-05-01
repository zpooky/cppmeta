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

template <typename Iterator>
Iterator do_parse(Iterator it, Iterator end, String &);

class StringTokenizer {
public:
  template <typename Iterator>
  Iterator parse(Iterator it, Iterator end, String &result) {
    return do_parse(it, end, result, '"');
  }
};

class CharacterTokenizer {
public:
  template <typename Iterator>
  Iterator parse(Iterator it, Iterator end, String &result) {
    return do_parse(it, end, result, '\'');
  }
};

template <typename Iterator>
Iterator do_parse(Iterator it, Iterator end, String &result, char c) {
  bool cont = true;
  while (it != end && cont) {
    char datum = *(it)++;
    if (datum == c) {
      if (!result.empty() && result.back() != '\\') {
        cont = false;
      }
    }
    result.push_back(datum);
  }
  return it;
}

#endif
