#ifndef SP_CPP_META_TOKENIZER_H
#define SP_CPP_META_TOKENIZER_H

#include "LineMeta.h"
#include "entities.h"
#include <vector>

template <typename Iterator>
Iterator do_parse(Iterator it, Iterator end, String &result, char c);

/*ITokenizer*/
class ITokenizer {
public:
  virtual ITokenizer *parse(LineMeta &, std::vector<Token> &) = 0;

  virtual ~ITokenizer() {
  }
};

/*Tokenizer*/
class Tokenizer {
private:
  File file;

public:
  Tokenizer(const File &p_file) : file(p_file) {
  }

  std::vector<Token> tokenize();
};

/*StringTokenizer*/
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

/*CharacterTokenizer*/
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

/*LineCommentTokenizer*/
class LineCommentTokenizer {
public:
  ITokenizer *parse(LineMeta &line, std::vector<Token> &) {
    if (line.data.starts_with("//")) {
      line.data.clear();
    }
    return nullptr;
  }
};

/*BlockCommentTokenizer*/
class BlockCommentTokenizer : public ITokenizer {
private:
public:
  ITokenizer *parse(LineMeta &line, std::vector<Token> &) {
    // const String begin("#<{(|");
    // if (line.data.starts_with(begin)) {
    const String end = "*/";
    ssize_t index = line.data.find(end);
    if (index != -1) {
      line.move_left(index + end.length());
      return nullptr;
    } else {
      line.data.clear();
    }
    // }
    return this;
  }
};

/*BaseTokenizer*/
class BaseTokenizer : public ITokenizer {
private:
  BlockCommentTokenizer blockTokenizer;

public:
  BaseTokenizer();

  ITokenizer *parse(LineMeta &, std::vector<Token> &);
};

/*Util*/
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
/**/
struct TokenResult {
  LineMeta &line;
  Location location;
  std::vector<Token> &result;
  TokenResult(std::vector<Token> &p_result, LineMeta &p_line)
      : line(p_line), location(0, Column(0, 0)), result(p_result) {
  }

  void push_back(String &token) {
    if (!token.empty()) {
      location.column.end = token.length();
      result.emplace_back(token, location);
      token.clear();
    }
    location = line.loc();
  }
};

#endif
