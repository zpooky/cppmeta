#ifndef SP_CPP_META_TOKENIZER_H
#define SP_CPP_META_TOKENIZER_H

#include "entities.h"
#include <array>
#include <vector>

struct LineMeta {
  const line_t number;
  String data;
  line_t column;

  LineMeta(line_t p_number, String &&p_data)
      : number(p_number), data(std::move(p_data)), column(0) {
  }

  auto begin() const {
    return data.begin();
  }

  auto end() const {
    return data.end();
  }

  void shrink_to(const char *it) {
    size_t number = data.shrink_to(it);
    column += number;
  }

  void move_left(size_t number) {
    data.move_left(number);
    column += number;
  }

  bool is_empty() {
    return data.empty();
  }

  char peek() const {
    return data[0];
  }

  char pop() {
    std::array<char, 1> chars;
    size_t length = data.drain_front(chars);
    if (length == 1) {
      ++column;
      return chars[0];
    }
    assert(false);
  }

  Location loc() const {
    return Location(number, Column(column, 0));
  }
};

class ITokenizer {
public:
  virtual ITokenizer *parse(LineMeta &, std::vector<Token> &) = 0;

  virtual ~ITokenizer() {
  }
};

class Tokenizer {
private:
  File file;
  ITokenizer *impl;

public:
  Tokenizer(const File &p_file) : file(p_file) {
  }

  std::vector<Token> tokenize();
};

template <typename Iterator>
Iterator do_parse(Iterator it, Iterator end, String &result, char c);

class StringTokenizer {
public:
  ITokenizer *parse(LineMeta &line, std::vector<Token> &result) {
    String token;
    auto it = do_parse(line.begin(), line.end(), token, '"');
    line.shrink_to(it);

    result.push_back(Token(token, line.number, line.column));
    return nullptr;
  }
};

class CharacterTokenizer {
public:
  ITokenizer *parse(LineMeta &line, std::vector<Token> &result) {
    String token;
    auto it = do_parse(line.begin(), line.end(), token, '\'');
    line.shrink_to(it);

    result.push_back(Token(token, line.number, line.column));
    return nullptr;
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

class LineCommentTokenizer {
public:
  ITokenizer *parse(LineMeta &line, std::vector<Token> &) {
    if (line.data.starts_with("//")) {
      line.data.clear();
    }
    return nullptr;
  }
};

class BlockCommentTokenizer : public ITokenizer {
public:
  ITokenizer *parse(LineMeta &line, std::vector<Token> &) {
    if (line.data.starts_with("/*")) {
      String end = "*/";
      ssize_t index = line.data.find(end);
      if (index != -1) {
        line.move_left(index + end.length());
        return nullptr;
      }
    }
    return this;
  }
};

class BaseTokenizer : public ITokenizer {
private:
  BlockCommentTokenizer blockTokenizer;
public:
  BaseTokenizer();
  ITokenizer *parse(LineMeta &, std::vector<Token> &);
};

#endif
