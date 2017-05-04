/*
 * wasd
 */
#include "tokenizer.h"
#include "String.h"
#include <array>
#include <forward_list>
#include <fstream>
#include <initializer_list>
#include <vector>

bool is_newline(char c);

void push_back(std::vector<Token> &, Location &, String &);
String read_line(std::ifstream &);

bool contains(char, const std::initializer_list<char> &);

// bool is_token(String &token, char datum, const String &compare);
bool is_token(LineMeta &, const std::initializer_list<char> &);
bool is_token(LineMeta &, const String &);
bool is_token(LineMeta &, char datum);
bool is_token(LineMeta &, const std::initializer_list<String> &);

BaseTokenizer::BaseTokenizer() : blockTokenizer() {
}

ITokenizer *BaseTokenizer::parse(LineMeta &line, std::vector<Token> &result) {
  Location location(0, Column(0, 0));
  String token;

  ITokenizer *tokenizer = nullptr;
  while (!line.is_empty()) {
    if (is_token(line, '"')) {
      push_back(result, location, token);

      StringTokenizer st;
      tokenizer = st.parse(line, result);

    } else if (is_token(line, '\'')) {
      push_back(result, location, token);

      CharacterTokenizer st;
      tokenizer = st.parse(line, result);

    } else if (is_token(line, {' ', '\t', '\r', char(0), '\n'})) {
      push_back(result, location, token);

    } else if (is_token(line, "//")) {
      push_back(result, location, token);

      LineCommentTokenizer tok;
      tokenizer = tok.parse(line, result);

    } else if (is_token(line, "/*")) {
      push_back(result, location, token);

      tokenizer = blockTokenizer.parse(line, result);

    } else if (is_token(line,
                        {"*/", "::", "==", "!=", "<=", ">=", "||", "&&"})) {
      push_back(result, location, token);
      // TODO

    } else if (is_token(line, {',', ';', '(', ')', '{', '}', '<', '>', '[', ']',
                               '.', '&', '~', '!', '|', '^', '%', '-', '+'})) {
      push_back(result, location, token);

      location = line.loc();
      token.push_back(line.pop());
      push_back(result, location, token);

    } else {
      if (token.empty()) {
        location = line.loc();
      }
      token.push_back(line.pop());
    }
  }
  push_back(result, location, token);
  if (tokenizer == nullptr) {
    tokenizer = this;
  }
  return tokenizer;
}

std::vector<Token> Tokenizer::tokenize() {
  std::ifstream fs;
  // fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fs.open(file.name.c_str());
  if (!fs.is_open()) {
    throw "wtf";
  }

  std::vector<Token> result;

  uint32_t lineno(0);

  BaseTokenizer baseTokenizer;
  ITokenizer *tokenizer = &baseTokenizer;
  while (!fs.eof()) {
    uint32_t column(0);
    Location location(lineno, Column(column, 0));

    String line = read_line(fs);
    LineMeta meta(lineno, std::move(line));

    while (!meta.is_empty()) {
      tokenizer = tokenizer->parse(meta, result);
    }

    ++lineno;
  }
  return result;
}

bool is_newline(char c) {
  return c == '\n';
}

void push_back(std::vector<Token> &result, Location &loc, String &token) {
  if (!token.empty()) {
    loc.column.end = token.length();
    result.emplace_back(token, loc);
    token.clear();
  }
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
