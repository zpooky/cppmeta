#ifndef SP_CPP_META_YAML_H
#define SP_CPP_META_YAML_H

#include "String.h"
#include <algorithm>
#include <list>
#include <vector>

namespace yaml {

using Key = String;
using Value = String;

class entry;
class yaml;
class Map;

/*List*/
class List {
private:
  std::list<entry> m_list;

public:
  List(const std::vector<Value> &);
  List(const std::vector<yaml> &);

  template <typename T>
  List(const std::vector<T> &collection) : m_list() {
    for (const auto &current : collection) {
      // m_list.emplace_back(current.to_yaml());
    }
  }

  String to_string(const String &) const;
};

/*yaml*/
class yaml {
private:
  std::list<entry> m_stack;

public:
  yaml();

  void push_back(const Key &, const yaml &);
  void push_back(const Key &, const Value &);
  void push_back(const Key &, const List &);

  String to_string(const String & = "") const;

  template <typename T>
  void push_back(const Key &, const T &);
  void push_back(const Key &, const std::string &) = delete;
};

template <typename T>
void yaml::push_back(const Key &key, const T &data) {
  push_back(key, data.to_yaml());
}

/*entry*/
enum class EType { LIST, MAP, SCALAR };
class entry {
private:
  Key m_key;
  union {
    List m_list;
    yaml m_map;
    Value m_scalar;
  };
  EType m_type;

public:
  entry(const Key &, List &&);
  entry(const Key &, const List &);
  entry(const Key &, Value &&);
  entry(const Key &, const Value &);
  entry(const Key &, const yaml &);

  entry(entry &&);
  entry(const entry &);

  ~entry();
  String to_string(const String &) const;
  EType type() const;

private:
};

} // namespace yaml

#endif
