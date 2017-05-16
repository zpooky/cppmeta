#ifndef SP_CPP_META_YAML_H
#define SP_CPP_META_YAML_H

#include "String.h"
#include <algorithm>
#include <list>
#include <vector>

namespace yaml {

using Key = String;
using Value = String;

/*KeyValue*/
class KeyValue {
private:
  Key m_key;
  Value m_value;

public:
  KeyValue(const Key &, const Value &);
  KeyValue(KeyValue &&);
  KeyValue(const KeyValue &);

  KeyValue &operator=(const KeyValue &) = delete;
  KeyValue &operator=(const KeyValue &&) = delete;

  String to_string(const String &) const;
};

class entry;
class yaml;
class Map;

/*List*/
class List {
private:
  Key m_key;
  std::list<entry> m_list;

public:
  List(const Key &, const std::vector<Value> &);
  List(const Key &, const std::vector<yaml> &);

  template <typename T>
  List(const Key &key, const std::vector<T> &collection) : m_key(key), m_list() {
    for (const auto &current : collection) {
      m_list.emplace_back(Map(Key("TODO"), current.to_yaml()));
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
/*Map*/
class Map {
private:
  Key m_key;
  yaml m_yaml;

public:
  Map(const Key &, const yaml &);
  Map(Map &&);
  Map(const Map &);

  Map &operator=(const Map &) = delete;
  Map &operator=(const Map &&) = delete;

  String to_string(const String &) const;
};

/*entry*/
enum class EType { KV, LIST, MAP, SCALAR };
class entry {
private:
  union {
    KeyValue m_kv;
    List m_list;
    Map m_map;
    Value m_scalar;
  };
  EType m_type;

public:
  entry(KeyValue &&);
  entry(List &&);
  entry(const List &);
  entry(Map &&);
  entry(Value &&);
  entry(const Value &);

  entry(entry &&);
  entry(const entry &);

  ~entry();
  String to_string(const String &) const;
  EType type() const;

private:
};

} // namespace yaml

#endif
