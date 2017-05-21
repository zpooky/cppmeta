#ifndef SP_CPP_META_TOKENIZER_H
#define SP_CPP_META_TOKENIZER_H

#include "LineMeta.h"
#include "TokenResult.h"
#include "tokens.h"
#include <vector>

void do_parse(LineMeta &, TokenResult &, char c);

/*ITokenizer*/
class ITokenizer {
public:
  virtual ITokenizer *parse(LineMeta &, TokenResult &) = 0;

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
  ITokenizer *parse(LineMeta &line, TokenResult &result) {
    do_parse(line, result, '"');

    return nullptr;
  }
};

/*CharacterTokenizer*/
class CharacterTokenizer {
public:
  ITokenizer *parse(LineMeta &line, TokenResult &result) {
    do_parse(line, result, '\'');

    return nullptr;
  }
};

/*LineCommentTokenizer*/
class LineCommentTokenizer {
public:
  ITokenizer *parse(LineMeta &line, TokenResult &) {
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
  ITokenizer *parse(LineMeta &line, TokenResult &) {
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

  ITokenizer *parse(LineMeta &, TokenResult &);
};


#endif
