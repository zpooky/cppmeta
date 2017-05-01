#ifndef SP_CPP_META_STRING_H
#define SP_CPP_META_STRING_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>

template <typename data_type, typename Allocator>
class BasicString {
private:
  static const data_type EMPTY_C_STRING[1];

  data_type *m_str;
  size_t m_index;
  size_t m_capacity;

  static_assert(std::is_trivially_copyable<data_type>::value, "");

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

  ~BasicString() {
    if (m_str) {
      Allocator a;
      a.deallocate(m_str, m_capacity + 1);
      m_str = nullptr;
      m_index = 0;
      m_capacity = 0;
    }
  }

  BasicString(BasicString<char, Allocator> &&o) : BasicString() {
    swap(o);
  }

  BasicString(const BasicString<char, Allocator> &o)
      : BasicString(o.m_str, o.m_index, o.m_capacity) {
  }

  BasicString<char, Allocator> &operator=(BasicString<char, Allocator> &&o) {
    swap(o);
    return *this;
  }

  BasicString<char, Allocator> &
  operator=(const BasicString<char, Allocator> &o) {
    swap(BasicString(o.m_str, o.m_index, o.m_capacity));
    return *this;
  }

  const data_type &operator[](size_t idx) const {
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
      swap(BasicString(m_str, m_index, realloc));
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

  bool operator==(const BasicString &o) const {
    return m_index == o.m_index && strncmp(m_str, o.m_str, m_index) == 0;
  }

  bool operator!=(const BasicString &o) const {
    return !operator==(o);
  }

  void push_back(const data_type &d) {
    if (d == data_type(0)) {
      throw "push_back\n";
    }
    require(1);
    m_str[m_index++] = d;
  }

  void append(const data_type *arr, size_t p_length) {
    require(p_length);
    std::copy(arr, arr + p_length, m_str + m_index);
    m_index += p_length;
  }

  template <size_t N>
  void append(const data_type (&str)[N]) {
    append(str, N);
  }

  void append(const BasicString &str) {
    append(str.m_str, str.m_index);
  }

  void append(const data_type *str) {
    append(str, strlen(str));
  }

  void append(const std::basic_string<data_type> &str) {
    append(str.data(), str.size());
  }

  BasicString<char, Allocator> substr(size_t start, size_t end) const {
    size_t e = std::min(m_capacity, end);
    if (start < e) {
      return {m_str + start, end - start};
    }
    return {};
  }

  BasicString<char, Allocator> substr(size_t start) const {
    return substr(start, length());
  }

  void clear() {
    m_index = 0;
  }

  void swap(BasicString<char, Allocator> &o) noexcept {
    std::swap(m_str, o.m_str);
    std::swap(m_index, o.m_index);
    std::swap(m_capacity, o.m_capacity);
  }

  void swap(BasicString<char, Allocator> &&o) noexcept {
    swap(o);
  }

  const data_type *c_str() const {
    if (m_str) {
      return m_str;
    }
    return EMPTY_C_STRING;
  }

  bool starts_with(const data_type *str, size_t length) const {
    if (m_index <= length) {
      return strncmp(m_str, str, length);
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

private:
  void require(size_t sz) {
    if (require_alloc(sz)) {
      size_t capacity = std::max(m_capacity * 2, m_capacity + sz);
      swap(BasicString(m_str, m_index, capacity));
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
