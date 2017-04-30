/*
 * wasd
 */
#include "tokenizer.h"
#include <array>
#include <fstream>
#include <string>
#include <vector>

bool is_blank(char c);
bool is_newline(char c);
bool is_inclusive_separator(std::string &token, char datum, std::string &out);
bool is_inclusive_separator(char);

void push_back(std::vector<Token> &, Location &, std::string &);
std::string read_line(std::ifstream &);

template <size_t N>
bool contains(char datum, const std::array<char, N> &);

std::vector<Token> Tokenizer::tokenize() {
  std::ifstream fs;
  // fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fs.open(file.name);
  if (!fs.is_open()) {
    throw "wtf";
  }

  std::vector<Token> result;

  uint32_t lineno(0);

  while (!fs.eof()) {
    uint32_t column(0);
    Location location(lineno, Column(column, 0));

    std::string token;
    std::string line = read_line(fs);

    auto it = line.begin();
    auto end = line.end();
    while (it != end) {
      auto begin = it;
      char datum = *(it)++;
      std::string out;

      if (datum == '"') {
        push_back(result, location, token);
        location = Location(lineno, Column(column, 0));

        StringTokenizer st;
        it = st.parse(begin, end, token);
        column += token.length();
        push_back(result, location, token);
        location = Location(lineno, Column(column, 0));
      } else if (datum == '\'') {
        push_back(result, location, token);
        location = Location(lineno, Column(column, 0));

        CharacterTokenizer st;
        it = st.parse(begin, end, token);
        column += token.length();
        push_back(result, location, token);
        location = Location(lineno, Column(column, 0));
      } else if (is_blank(datum)) {
        push_back(result, location, token);
        location = Location(lineno, Column(column, 0));
      } else if (is_inclusive_separator(token, datum, out)) {
        size_t token_length = token.length();
        push_back(result, location, token);

        location =
            Location(lineno, Column((column + token_length) - out.length(), 0));
        push_back(result, location, out);

        location = Location(lineno, Column(column + 1, 0));
      } else if (is_inclusive_separator(datum)) {
        push_back(result, location, token);

        location = Location(lineno, Column(column, 0));
        token.push_back(datum);
        push_back(result, location, token);

        location = Location(lineno, Column(column + 1, 0));
      } else {
        token.push_back(datum);
      }
      ++column;
    }
    push_back(result, location, token);

    ++lineno;
  }
  return result;
}

bool is_blank(char c) {
  return c == ' ' || c == '\t' || c == '\r';
}

bool is_newline(char c) {
  return c == '\n';
}

bool end_with(const std::string &token, const std::string &end) {
  size_t len = token.length();
  if (len > end.length()) {
    std::string sub = token.substr(len - end.length());
    return sub == end;
  }
  return false;
}

bool is_inclusive_separator(std::string &token, char datum, std::string &out) {
  std::string joined;
  joined.resize(token.size() + sizeof(datum));
  joined.append(token);
  joined.push_back(datum);

  std::array<std::string, 3> pool{"/*", "*\\", "//"};
  for (const auto &p : pool) {
    if (end_with(joined, p)) {
      token = joined.substr(0, joined.length() - p.length());
      out = p;
      return true;
    }
  }
  return false;
}

bool is_inclusive_separator(char datum) {
  std::array<char, 12> separtors{',', ';', '(', ')', '{', '}',
                                 '<', '>', '[', ']', '.', '&'};
  return contains(datum, separtors);
}

void push_back(std::vector<Token> &result, Location &loc, std::string &token) {
  if (!token.empty()) {
    loc.column.end = token.length();
    result.emplace_back(token, loc);
    token.clear();
  }
}
std::string read_line(std::ifstream &fs) {
  std::string result;

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

template <size_t N>
bool contains(char datum, const std::array<char, N> &pool) {
  for (const auto &p : pool) {
    if (datum == p) {
      return true;
    }
  }
  return false;
}
