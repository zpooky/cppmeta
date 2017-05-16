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
struct Types;

/*List*/
class List {
private:
  std::list<Types> m_list;

public:
  List(const std::vector<Value> &);
  List(const std::vector<yaml> &);

  template <typename T>
  List(const std::vector<T> &collection) : m_list() {
    for (const auto &current : collection) {
      m_list.emplace_back(current.to_yaml());
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
enum class EType { LIST, MAP, KV, SCALAR };
struct Types {
  union {
    List m_list;
    yaml m_map;
    Value m_scalar;
  };
  EType type;

  Types(const Types &);
  Types(Types &&);

  Types(List &&);
  Types(const List &);

  Types(Value &&);
  Types(const Value &);

  Types(const yaml &);

  ~Types();

  String to_string(const String &) const;
};

class entry {
private:
  Types m_type;
  Key m_key;

public:
  template <typename T>
  entry(const Key &key, T &&value)
      : m_key(key), m_type(std::forward<T>(value)) {
  }

  entry(entry &&);
  entry(const entry &);

  ~entry();
  String to_string(const String &) const;

private:
};

} // namespace yaml

#endif
