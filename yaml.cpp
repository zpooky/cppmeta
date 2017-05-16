#include "yaml.h"
#include <cassert>

namespace yaml {
/*KeyValue*/
KeyValue::KeyValue(const Key &key, const Value &value)
    : m_key(key), m_value(value) {
}

KeyValue::KeyValue(KeyValue &&o)
    : m_key(std::move(o.m_key)), m_value(std::move(o.m_value)) {
}

KeyValue::KeyValue(const KeyValue &o) : KeyValue(o.m_key, o.m_value) {
}

String KeyValue::to_string(const String &prefix) const {
  String result;
  result.append(prefix).append(m_key).append(String(": "));
  result.append(m_value).append("\n");
  return result;
}

/*List*/
List::List(const Key &key, const std::vector<Value> &collection)
    : m_key(key), m_list{} {
  for (const auto &current : collection) {
    m_list.emplace_back(current);
  }
}

List::List(const Key &key, const std::vector<yaml> &collection)
    : m_key(key), m_list{} {
  for (const auto &current : collection) {
    m_list.emplace_back(Map(Key("TODO"), current));
  }
}

String List::to_string(const String &prefix) const {
  String result;
  for (const auto &current : m_list) {
    if (current.type() == EType::KV) {
      String cprfx(prefix);
      cprfx.append("-");
      result.append(current.to_string(cprfx));
    } else {
      result.append(current.to_string(prefix));
    }
  }
  return result;
}

/*yaml*/
/*public*/
yaml::yaml() : m_stack() {
}

void yaml::push_back(const Key &key, const yaml &value) {
  m_stack.emplace_back(Map(key, value));
}

void yaml::push_back(const Key &key, const Value &value) {
  m_stack.emplace_back(KeyValue(key, value));
}

void yaml::push_back(const Key &, const List &value) {
  m_stack.emplace_back(value);
}

String yaml::to_string(const String &prefix) const {
  String result;
  for (const entry &current : m_stack) {
    result.append(current.to_string(prefix));
  }
  return result;
}
/*private*/

/*Map*/
/*public*/
Map::Map(const Key &p_key, const yaml &p_value)
    : m_key(p_key), m_yaml(p_value) {
}

Map::Map(Map &&o) : m_key(std::move(o.m_key)), m_yaml(std::move(o.m_yaml)) {
}

Map::Map(const Map &o) : m_key(o.m_key), m_yaml(o.m_yaml) {
}

String Map::to_string(const String &prefix) const {
  String result;
  result.append(prefix).append(m_key).append(":\n");
  result.append(m_yaml.to_string(prefix + "  "));
  return result;
}
/*entry*/
/*public*/
entry::entry(KeyValue &&kv) : m_kv(std::move(kv)), m_type(EType::KV) {
}

entry::entry(List &&l) : m_list(std::move(l)), m_type(EType::LIST) {
}

entry::entry(const List &l) : m_list(std::move(l)), m_type(EType::LIST) {
}

entry::entry(Map &&p_map) : m_map(std::move(p_map)), m_type(EType::MAP) {
}

entry::entry(Value &&v) : m_scalar(std::move(v)), m_type(EType::SCALAR) {
}

entry::entry(const Value &v) : m_scalar(v), m_type(EType::SCALAR) {
}

entry::entry(entry &&o) : m_type(o.m_type) {
  if (m_type == EType::KV) {
    new (&m_kv) KeyValue(std::move(o.m_kv));
  }
  if (m_type == EType::LIST) {
    new (&m_list) List(std::move(o.m_list));
  }
  if (m_type == EType::MAP) {
    new (&m_map) Map(std::move(o.m_map));
  }
  if (m_type == EType::SCALAR) {
    new (&m_scalar) String(std::move(o.m_scalar));
  }
}

entry::entry(const entry &o) : m_type(o.m_type) {
  if (m_type == EType::KV) {
    new (&m_kv) KeyValue(o.m_kv);
  }
  if (m_type == EType::LIST) {
    new (&m_list) List(o.m_list);
  }
  if (m_type == EType::MAP) {
    new (&m_map) Map(o.m_map);
  }
  if (m_type == EType::SCALAR) {
    new (&m_scalar) String(o.m_scalar);
  }
}

entry::~entry() {
  if (m_type == EType::KV) {
    m_kv.~KeyValue();
  }
  if (m_type == EType::LIST) {
    m_list.~List();
  }
  if (m_type == EType::MAP) {
    m_map.~Map();
  }
  if (m_type == EType::SCALAR) {
    m_scalar.~Value();
  }
}

String entry::to_string(const String &prefix) const {
  if (m_type == EType::KV) {
    return m_kv.to_string(prefix);
  }
  if (m_type == EType::LIST) {
    return m_list.to_string(prefix);
  }
  if (m_type == EType::MAP) {
    return m_map.to_string(prefix);
  }
  if (m_type == EType::SCALAR) {
    String result(prefix);
    result.append(m_scalar);
    return result;
  }
}

EType entry::type() const {
  return m_type;
}
/*private*/

} // namespace yaml
