#ifndef SP_CPP_META_ENTITIES_H
#define SP_CPP_META_ENTITIES_H

#include "String.h"
#include "yaml.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

using column_t = uint32_t;
using line_t = uint32_t;

struct Column {
  column_t begin;
  column_t end;

  Column() : begin(0), end(0) {
  }

  Column(column_t p_begin, column_t p_end) : begin(p_begin), end(p_end) {
  }

  // Column(const Column &o) : Column(o.begin, o, end) {
  // }
  //
  // Column(Column &&o) : Column() {
  //   swap(o);
  // }

  void swap(Column &o) {
    std::swap(o.begin, begin);
    std::swap(o.end, end);
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

  auto to_yaml() const {
    yaml::yaml result;
    result.push_back("begin", String(std::to_string(begin)));
    result.push_back("end", String(std::to_string(end)));
    return result;
  }
};

struct Location {
  line_t line;
  Column column;
  Location() : line(0), column(Column(0, 0)) {
  }

  Location(line_t p_line, const Column &p_column)
      : line(p_line), column(p_column) {
  }

  Location(const Location &o) : line(o.line), column(o.column) {
  }

  Location(Location &&o) : Location() {
    swap(o);
  }

  // Location &operator=(Location o) {
  //   swap(o);
  //   return *this;
  // }

  Location &operator=(Location &&o) {
    swap(o);
    return *this;
  }

  void swap(Location &o) {
    std::swap(line, o.line);
    std::swap(column, o.column);
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

  auto to_yaml() const {
    yaml::yaml result;
    result.push_back("line", String(std::to_string(line)));
    result.push_back("column", column);
    return result;
  }
};

struct Token {
  String token;
  Location location;

  Token() : token(), location() {
  }

  Token(const String &p_token) : Token(p_token, Location()) {
  }

  Token(const String &p_token, const Location &p_loc)
      : token(p_token), location(p_loc) {
  }
  Token(const String &p_token, line_t p_line, column_t p_start)
      : token(p_token), location(p_line, Column(p_start, p_token.length())) {
  }

  Token(const Token &o) : token(o.token), location(o.location) {
  }

  Token(Token &&o) : Token() {
    swap(o);
  }

  Token &operator=(Token o) {
    swap(o);
    return *this;
  }

  Token &operator=(Token &&o) {
    swap(o);
    return *this;
  }

  bool operator==(const char *t) const {
    return std::strcmp(token.c_str(), t) == 0;
  }
  bool operator==(const String &t) const {
    return token == t;
  }

  void swap(Token &o) {
    std::swap(token, o.token);
    location.swap(o.location);
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

  auto to_yaml() const {
    yaml::yaml result;
    result.push_back("token", token);
    result.push_back("location", location);
    return result;
  }
};

struct File {
  String name;
  File(const String &p_name) : name(p_name) {
  }
};
#endif
