#ifndef SP_CPP_META_LINE_META_H
#define SP_CPP_META_LINE_META_H

#include "String.h"
#include "entities.h"
#include <array>

struct LineMeta {
  const line_t number;
  String data;
  line_t column;

  LineMeta(line_t p_number, String &&p_data)
      : number(p_number), data(std::move(p_data)), column(0) {
  }

  auto begin() const {
    return data.begin();
  }

  auto end() const {
    return data.end();
  }

  void shrink_to(const char *it) {
    size_t number = data.shrink_to(it);
    column += number;
  }

  void move_left(size_t number) {
    data.move_left(number);
    column += number;
  }

  bool is_empty() {
    return data.empty();
  }

  char peek() const {
    return data[0];
  }

  char pop() {
    std::array<char, 1> chars;
    size_t length = data.drain_front(chars);
    if (length == 1) {
      ++column;
      return chars[0];
    }
    assert(false);
  }

  Location loc() const {
    return Location(number, Column(column, 0));
  }
};

#endif
