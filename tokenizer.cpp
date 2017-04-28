#include "tokenizer.h"
#include <fstream>
#include <string>
#include <vector>

void push_back(std::vector<Token> &, Location &, std::string &);

bool Tokenizer::is_blank(char c) const {
  return c == ' ' || c == '\t' || c == '\r' || is_newline(c);
}

bool Tokenizer::is_newline(char c) const {
  return c == '\n';
}

std::vector<Token> Tokenizer::tokenize() {
  std::ifstream fs;
  // fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fs.open(file.name);
  if(!fs.is_open()){
    throw "wtf";
  }

  std::vector<Token> result;

  uint32_t line(0);
  uint32_t column(0);
  std::string token;

  Location location(line, Column(column, 0));

  char datum = fs.get();
  while (fs.good()) {
    if (is_blank(datum)) {
      push_back(result, location, token);
      location = Location(line, Column(column, 0));
    } else {
      token.push_back(datum);
    }

    if (is_newline(datum)) {
      ++line;
      column = 0;
    } else {
      ++column;
    }

    datum = fs.get();
  }

  push_back(result, location, token);
  return result;
}

void push_back(std::vector<Token> &result, Location &location,
               std::string &token) {
  if (!token.empty()) {
    location.column.end = token.length();
    result.emplace_back(token, location);
    token.clear();
  }
}


