#ifndef SP_CPP_META_STRING_H
#define SP_CPP_META_STRING_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <string.h>
#include <type_traits>

template <typename data_type, typename Allocator>
class BasicString {
private:
  static const data_type EMPTY_C_STRING[1];

  data_type *m_str;
  size_t m_index;
  size_t m_capacity;

  static_assert(std::is_trivially_copyable<data_type>::value, "");
  using SelfType = BasicString<data_type, Allocator>;

public:
  constexpr BasicString() : m_str(nullptr), m_index(0), m_capacity(0) {
  }

  explicit BasicString(size_t capacity) : BasicString() {
    m_capacity = capacity;
    Allocator a;
    m_str = a.allocate(m_capacity + 1); //+1 space for cstr 0
    memset(m_str, 0, m_capacity + 1);
  }

  BasicString(const data_type *o, size_t length, size_t capacity)
      : BasicString(capacity) {
    append(o, length);
  }

  BasicString(const data_type *o, size_t length)
      : BasicString(o, length, length) {
  }

  template <size_t N>
  BasicString(const data_type (&str)[N]) : BasicString(str, N) {
  }

  BasicString(const data_type *str) : BasicString(str, strlen(str)) {
  }

  BasicString(const std::basic_string<data_type> &str)
      : BasicString(str.data(), str.size()) {
  }

  ~BasicString() {
    if (m_str) {
      Allocator a;
      a.deallocate(m_str, m_capacity + 1);
      m_str = nullptr;
      m_index = 0;
      m_capacity = 0;
    }
  }

  BasicString(SelfType &&o) : BasicString() {
    swap(o);
  }

  BasicString(const SelfType &o)
      : BasicString(o.m_str, o.m_index, o.m_capacity) {
  }

  BasicString<char, Allocator> &operator=(SelfType &&o) {
    swap(o);
    return *this;
  }

  SelfType &operator=(const SelfType &o) {
    swap(SelfType(o.m_str, o.m_index, o.m_capacity));
    return *this;
  }

  const data_type at(size_t idx) const {
    if (idx < m_index) {
      return m_str[idx];
    }
    return 0;
  }
  const data_type operator[](size_t idx) const {
    assert(idx < m_index);
    return m_str[idx];
  }

  data_type &operator[](size_t idx) {
    assert(idx < m_index);
    return m_str[idx];
  }

  void resize(size_t sz) {
    size_t realloc = std::max(sz, m_index);
    if (realloc != sz) {
      swap(SelfType(m_str, m_index, realloc));
    }
  }

  data_type *begin() {
    return m_str;
  }

  const data_type *begin() const {
    return m_str;
  }

  data_type *end() {
    return m_str ? m_str + m_index : nullptr;
  }

  const data_type *end() const {
    return m_str ? m_str + m_index : nullptr;
  }

  size_t length() const {
    return m_index;
  }

  bool empty() const {
    return m_index == 0;
  }

  data_type &back() {
    assert(m_index != 0);
    return m_str[m_index - 1];
  }

  const data_type &back() const {
    assert(m_index != 0);
    return m_str[m_index - 1];
  }

  bool operator==(const SelfType &o) const {
    return m_index == o.m_index && strncmp(m_str, o.m_str, m_index) == 0;
  }

  bool operator!=(const SelfType &o) const {
    return !operator==(o);
  }

  void push_back(const data_type &d) {
    if (d == data_type(0)) {
      throw "push_back\n";
    }
    require(1);
    m_str[m_index++] = d;
  }

   SelfType& append(const data_type *arr, size_t p_length) {
    require(p_length);
    std::copy(arr, arr + p_length, m_str + m_index);
    m_index += p_length;
    return *this;
  }

  template <size_t N>
  SelfType& append(const data_type (&str)[N]) {
    return append(str, N);
  }

  SelfType &append(const SelfType &str) {
    return append(str.m_str, str.m_index);
  }

  SelfType& append(const data_type *str) {
    return append(str, strlen(str));
  }

  SelfType& append(const std::basic_string<data_type> &str) {
    return append(str.data(), str.size());
  }

  SelfType substr(size_t start, size_t end) const {
    size_t e = std::min(m_capacity, end);
    if (start < e) {
      return {m_str + start, end - start};
    }
    return {};
  }

  SelfType substr(size_t start) const {
    return substr(start, length());
  }

  void clear() {
    memset(m_str, 0, m_index);
    m_index = 0;
  }

  void swap(SelfType &o) noexcept {
    std::swap(m_str, o.m_str);
    std::swap(m_index, o.m_index);
    std::swap(m_capacity, o.m_capacity);
  }

  void swap(SelfType &&o) noexcept {
    swap(o);
  }

  const data_type *c_str() const {
    if (m_str) {
      return m_str;
    }
    return EMPTY_C_STRING;
  }

  bool starts_with(const data_type *str, size_t length) const {
    if (length <= m_index) {
      return strncmp(m_str, str, length) == 0;
    }
    return false;
  }

  template <size_t N>
  bool starts_with(const data_type (&str)[N]) const {
    return starts_with(str, N);
  }

  bool starts_with(const data_type *str) const {
    return starts_with(str, strlen(str));
  }

  bool starts_with(const SelfType &str) const {
    return starts_with(str.m_str, str.m_index);
  }

  bool ends_with(const data_type *end, size_t p_length) const {
    if (length() >= p_length) {
      const data_type *this_end = m_str - (p_length - length());
      return strcmp(this_end, end) == 0;
    }
    return false;
  }

  bool ends_with(const data_type *end) const {
    return ends_with(end, strlen(end));
  }

  bool ends_with(const SelfType &end) const {
    return ends_with(end.m_str, end.length());
  }

  template <size_t N>
  bool ends_with(const data_type (&str)[N]) const {
    return ends_with(str, N);
  }

  void drop_right(size_t p_length) {
    size_t mask = std::min(p_length, length());
    size_t offset = length() - mask;
    memset(m_str + offset, 0, length() - offset);
    m_index = offset;
  }

  template <size_t N>
  size_t drain_front(std::array<data_type, N> &drain) {
    size_t length = std::min(N, m_index);
    memcpy(drain.data(), m_str, length);
    move_left(length);
    return length;
  }

  void move_left(size_t length) {
    size_t len = std::min(length, m_index);
    size_t remaining = m_index - len;
    memmove(m_str, m_str + len, remaining);
    memset(m_str + remaining, 0, m_index - remaining);
    m_index = remaining;
  }

  size_t shrink_to(const data_type *it) {
    size_t idx = it - m_str;
    move_left(idx);
    return idx;
  }

  ssize_t find(const data_type *needle, size_t length) const {
    if (length == 0) {
      return 0;
    }
    size_t index = m_index;
    for (size_t i(0); i < m_index; ++i) {
      if (length <= index--) {
        if (memcmp(m_str + i, needle, length) == 0) {
          return i;
        }
      }
    }
    return -1;
  }

  ssize_t find(const data_type *needle) const {
    return find(needle, strlen(needle));
  }

  template <size_t N>
  ssize_t find(const data_type (&str)[N]) const {
    return find(str, N);
  }

  ssize_t find(const SelfType &needle) const {
    return find(needle.m_str, needle.m_index);
  }

private:
  void require(size_t sz) {
    if (require_alloc(sz)) {
      size_t capacity = std::max(m_capacity * 2, m_capacity + sz);
      swap(SelfType(m_str, m_index, capacity));
    }
  }

  bool require_alloc(size_t sz) const {
    return (m_index + sz) > m_capacity;
  }
};
template <typename data_type, typename Allocator>
const data_type BasicString<data_type, Allocator>::EMPTY_C_STRING[1] = {0};

using String = BasicString<char, std::allocator<char>>;

template <typename Allocator>
std::ostream &operator<<(std::ostream &os,
                         const BasicString<char, Allocator> &str) {
  os << str.c_str();
  return os;
}

#endif
