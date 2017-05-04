#ifndef SP_CPP_META_ENTITIES_H
#define SP_CPP_META_ENTITIES_H

#include "String.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

using column_t = uint32_t;
using line_t = uint32_t;

struct Column {
  column_t begin;
  column_t end;

  Column(column_t p_begin, column_t p_end) : begin(p_begin), end(p_end) {
  }

  String to_string() const {
    String result;
    result.append("[");
    result.append("b:");
    result.append(std::to_string(begin));
    result.append(",");
    result.append("e:");
    result.append(std::to_string(end));
    result.append("]");
    return result;
  }
};

struct Location {
  line_t line;
  Column column;
  Location(line_t p_line, const Column &p_column)
      : line(p_line), column(p_column) {
  }
  String to_string() const {
    String result;
    result.append("[");
    result.append("l:");
    result.append(std::to_string(line));
    result.append(",");
    result.append("col:");
    result.append(column.to_string());
    result.append("]");
    return result;
  }
};

struct Token {
  String token;
  Location location;
  Token(const String &p_token, const Location &p_loc)
      : token(p_token), location(p_loc) {
  }
  Token(const String &p_token, line_t p_line, column_t p_start)
      : token(p_token), location(p_line, Column(p_start, p_token.length())) {
  }

  bool operator==(const char *t) const {
    return std::strcmp(token.c_str(), t) == 0;
  }
  bool operator==(const String &t) const {
    return token == t;
  }

  String to_string() const {
    String result;
    result.append("[");
    result.append(token);
    result.append(",");
    result.append("loc:");
    result.append(location.to_string());
    result.append("]");
    return result;
  }
};

struct File {
  String name;
  File(const String &p_name) : name(p_name) {
  }
};
#endif
