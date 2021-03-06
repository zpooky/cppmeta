/*
 * wasd
 */
#include "tokenizer.h"
#include "String.h"
#include <array>
#include <forward_list>
#include <fstream>
#include <initializer_list>
#include "ArrayList.h"

/*Util*/
bool is_newline(char c);

String read_line(std::ifstream &);

bool contains(char, const std::initializer_list<char> &);

bool is_token(LineMeta &, const std::initializer_list<char> &);
bool is_token(LineMeta &, const String &);
bool is_token(LineMeta &, char datum);
bool is_token(LineMeta &, const std::initializer_list<String> &);

/*BaseTokenizer*/
BaseTokenizer::BaseTokenizer() : blockTokenizer() {
}

ITokenizer *BaseTokenizer::parse(LineMeta &line, TokenResult &result) {
  String token;

  ITokenizer *tokenizer = nullptr;
  while (!line.is_empty()) {
    if (is_token(line, '"')) {
      result.push_back(token);

      StringTokenizer st;
      tokenizer = st.parse(line, result);

    } else if (is_token(line, '\'')) {
      result.push_back(token);

      CharacterTokenizer st;
      tokenizer = st.parse(line, result);

    } else if (is_token(line, {' ', '\t', '\r', char(0), '\n'})) {
      result.push_back(token);
      line.pop();

    } else if (is_token(line, "//")) {
      result.push_back(token);

      LineCommentTokenizer tok;
      tokenizer = tok.parse(line, result);

    } else if (is_token(line, "/*")) {
      result.push_back(token);

      tokenizer = blockTokenizer.parse(line, result);

    } else if (is_token(line, {"*/", "::", "==", "!=", "<=", ">=", "||", /*"&&",*/ "--", "++" /*, "<<", ">>"*/})) {
      result.push_back(token);
      // assuming its only match 2 length token
      token.push_back(line.pop());
      token.push_back(line.pop());
      result.push_back(token);

    } else if (is_token(line, {'=', ',', ';', '(', ')', '{',  '}', '<', '*', '>', '[', ']', '.', '&', '~',  '!', '|', '^', '%', '-', '+', '#', ':', '\\', '/'})) {
      result.push_back(token);

      token.push_back(line.pop());
      result.push_back(token);

    } else {
      token.push_back(line.pop());
    }
  }
  result.push_back(token);
  return tokenizer;
}
/*Tokenizer*/
sp::ArrayList<Token> Tokenizer::tokenize() {
  std::ifstream fs;
  // fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fs.open(file.name.c_str());
  if (!fs.is_open()) {
    throw "wtf";
  }

  sp::ArrayList<Token> tokens;

  uint32_t lineno(0);

  BaseTokenizer baseTokenizer;
  ITokenizer *tokenizer = &baseTokenizer;
  while (!fs.eof()) {
    uint32_t column(0);
    Location location(lineno, Column(column, 0));

    String line = read_line(fs);
    LineMeta meta(lineno, std::move(line));
    TokenResult result(tokens, meta);

    while (!meta.is_empty()) {
      tokenizer = tokenizer->parse(meta, result);
      if (tokenizer == nullptr) {
        tokenizer = &baseTokenizer;
      }
    }

    ++lineno;
  }
  return tokens;
}

/*Util*/
bool is_newline(char c) {
  return c == '\n';
}

String read_line(std::ifstream &fs) {
  String result;

  char datum = fs.get();
  while (fs.good()) {
    if (is_newline(datum)) {
      break;
    } else {
      result.push_back(datum);
    }
    datum = fs.get();
  }

  return result;
}

bool contains(char datum, const std::initializer_list<char> &pool) {
  for (const auto &p : pool) {
    if (datum == p) {
      return true;
    }
  }
  return false;
}

bool is_token(LineMeta &line, const std::initializer_list<char> &tokens) {
  return contains(line.peek(), tokens);
}

bool is_token(LineMeta &line, const String &token) {
  bool result = true;
  for (size_t i(0); i < token.length(); ++i) {
    result &= line.data.at(i) == token[i];
  }
  return result;
}

bool is_token(LineMeta &line, char datum) {
  return line.peek() == datum;
}

bool is_token(LineMeta &line, const std::initializer_list<String> &pool) {
  for (const auto &token : pool) {
    if (is_token(line, token)) {
      return true;
    }
  }
  return false;
}

void do_parse(LineMeta &line, TokenResult &result, char c) {
  if (line.peek() == c) {
    String token;
    token.push_back(line.pop());
    result.push_back(token);

    while (!line.is_empty()) {
      char datum = line.pop();
      if (datum == c) {
        if (!token.is_empty() && token.back() == '\\') {
          token.push_back(datum);
        } else {
          result.push_back(token);
          token.push_back(datum);
          break;
        }
      } else {
        token.push_back(datum);
      }
    }
    result.push_back(token);
  }
}
