#include "yaml.h"
#include <cassert>

namespace yaml {
/*List*/
List::List(const std::vector<Value> &collection) : m_list{} {
  for (const auto &current : collection) {
    // m_list.emplace_back(current);
  }
}

List::List(const std::vector<yaml> &collection) : m_list{} {
  for (const auto &current : collection) {
    // m_list.emplace_back(current);
  }
}

String List::to_string(const String &prefix) const {
  String result;
  for (const auto &current : m_list) {
    String cprfx(prefix);
    cprfx.append("-");
    result.append(current.to_string(prefix));
  }
  return result;
}

/*yaml*/
/*public*/
yaml::yaml() : m_stack() {
}

void yaml::push_back(const Key &key, const yaml &value) {
  m_stack.emplace_back(key, value);
}

void yaml::push_back(const Key &key, const Value &value) {
  m_stack.emplace_back(key, value);
}

void yaml::push_back(const Key &key, const List &value) {
  m_stack.emplace_back(key, value);
}

String yaml::to_string(const String &prefix) const {
  String result;
  for (const entry &current : m_stack) {
    result.append(current.to_string(prefix));
  }
  return result;
}
/*private*/

/*entry*/
/*public*/

entry::entry(const Key &key, List &&l)
    : m_key(key), m_list(std::move(l)), m_type(EType::LIST) {
}

entry::entry(const Key &key, const List &l)
    : m_key(key), m_list(std::move(l)), m_type(EType::LIST) {
}

entry::entry(const Key &key, Value &&v)
    : m_key(key), m_scalar(std::move(v)), m_type(EType::SCALAR) {
}

entry::entry(const Key &key, const Value &v)
    : m_key(key), m_scalar(v), m_type(EType::SCALAR) {
}

entry::entry(const Key &key, const yaml &v)
    : m_key(key), m_map(v), m_type(EType::MAP) {
}

entry::entry(entry &&o) : m_key(std::move(o.m_key)), m_type(o.m_type) {
  if (m_type == EType::LIST) {
    new (&m_list) List(std::move(o.m_list));
  }
  if (m_type == EType::MAP) {
    new (&m_map) yaml(std::move(o.m_map));
  }
  if (m_type == EType::SCALAR) {
    new (&m_scalar) String(std::move(o.m_scalar));
  }
}

entry::entry(const entry &o) : m_key(o.m_key), m_type(o.m_type) {
  if (m_type == EType::LIST) {
    new (&m_list) List(o.m_list);
  }
  if (m_type == EType::MAP) {
    new (&m_map) yaml(o.m_map);
  }
  if (m_type == EType::SCALAR) {
    new (&m_scalar) String(o.m_scalar);
  }
}

entry::~entry() {
  if (m_type == EType::LIST) {
    m_list.~List();
  }
  if (m_type == EType::MAP) {
    m_map.~yaml();
  }
  if (m_type == EType::SCALAR) {
    m_scalar.~Value();
  }
}

String entry::to_string(const String &prefix) const {
  String result(prefix);
  result.append(m_key);
  result.append(": ");
  if (m_type == EType::LIST) {
    String nprefix(prefix);
    nprefix.append("  ");

    result.append("\n");
    result.append(m_list.to_string(nprefix));
  } else if (m_type == EType::MAP) {
    String nprefix(prefix);
    nprefix.append("  ");

    result.append("\n");
    result.append(m_map.to_string(nprefix));
  } else if (m_type == EType::SCALAR) {
    result.append(m_scalar);
    result.append("\n");
  }
  return result;
}

EType entry::type() const {
  return m_type;
}
/*private*/

} // namespace yaml
