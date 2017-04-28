#ifndef SP_CPP_META_ENTITIES_H
#define SP_CPP_META_ENTITIES_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <cstring>

using column_t = uint32_t;
using line_t = uint32_t;

struct Column {
  column_t begin;
  column_t end;
  Column(column_t p_begin, column_t p_end) : begin(p_begin), end(p_end) {
  }
};

struct Location {
  line_t line;
  Column column;
  Location(line_t p_line, const Column &p_column)
      : line(p_line), column(p_column) {
  }
};

struct Token {
  std::string token;
  Location location;
  Token(const std::string &p_token, const Location &p_loc)
      : token(p_token), location(p_loc) {
  }

  bool operator==(const char *t) const {
    return std::strcmp(token.c_str(), t) == 0;
  }
  bool operator==(const std::string &t) const {
    return token == t;
  }
};

struct File {
  std::string name;
  File(const std::string &p_name) : name(p_name) {
  }
};
#endif
