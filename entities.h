#ifndef SP_CPP_META_H
#define SP_CPP_META_H

#include <cstddef>
#include <cstdint>
#include <string>

struct Column {
  uint32_t begin;
  uint32_t end;
  Column(uint32_t p_begin, uint32_t p_end) : begin(p_begin), end(p_end) {
  }
};

struct Location {
  uint32_t line;
  Column column;
  Location(uint32_t p_line, const Column &p_column)
      : line(p_line), column(p_column) {
  }
};

struct Token {
  std::string token;
  Location location;
  Token(const std::string &p_token, const Location &p_loc)
      : token(p_token), location(p_loc) {
  }
};

struct File {
  std::string name;
  File(const std::string&p_name):name(p_name){
  }
};
#endif
