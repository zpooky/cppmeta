#include "yaml.h"
#include <cassert>

namespace yaml {
/*List*/
List::List(const std::vector<Value> &collection) : m_list{} {
  for (const auto &current : collection) {
    m_list.emplace_back(current);
  }
}

List::List(const std::vector<yaml> &collection) : m_list{} {
  for (const auto &current : collection) {
    m_list.emplace_back(current);
  }
}

String List::to_string(const String &prefix) const {
  String result;
  for (const auto &current : m_list) {
    result.append(prefix);
    result.append("- ");
    result.append(current.to_string(prefix));
  }
  return result;
}

bool List::is_empty() const {
  return m_list.empty();
}
/*yaml*/
/*public*/
yaml::yaml() : m_stack() {
}

void yaml::push_back(const Key &key, const yaml &value) {
  if (!value.is_empty()) {
    m_stack.emplace_back(key, value);
  }
}

void yaml::push_back(const Key &key, const Value &value) {
  m_stack.emplace_back(key, value);
}

void yaml::push_back(const Key &key, const List &value) {
  if (!value.is_empty()) {
    m_stack.emplace_back(key, value);
  }
}

bool yaml::is_empty() const {
  return m_stack.empty();
}

String yaml::to_string(const String &prefix) const {
  String result;
  for (const entry &current : m_stack) {
    result.append(current.to_string(prefix));
  }
  return result;
}
/*private*/

/*public*/
Types::Types(const Types &o) : type(o.type) {
  if (type == EType::LIST) {
    new (&m_list) List(o.m_list);
  }
  if (type == EType::MAP) {
    new (&m_map) yaml(o.m_map);
  }
  if (type == EType::KV || type == EType::SCALAR) {
    new (&m_scalar) String(o.m_scalar);
  }
}

Types::Types(Types &&o) : type(o.type) {
  if (type == EType::LIST) {
    new (&m_list) List(std::move(o.m_list));
  }
  if (type == EType::MAP) {
    new (&m_map) yaml(std::move(o.m_map));
  }
  if (type == EType::KV || type == EType::SCALAR) {
    new (&m_scalar) String(std::move(o.m_scalar));
  }
}

Types::Types(List &&l) : m_list(std::move(l)), type(EType::LIST) {
}

Types::Types(const List &l) : m_list(std::move(l)), type(EType::LIST) {
}

Types::Types(Value &&v) : m_scalar(std::move(v)), type(EType::KV) {
}

Types::Types(const Value &v) : m_scalar(v), type(EType::KV) {
}

Types::Types(const yaml &v) : m_map(v), type(EType::MAP) {
}

Types::~Types() {
  if (type == EType::LIST) {
    m_list.~List();
  }
  if (type == EType::MAP) {
    m_map.~yaml();
  }
  if (type == EType::KV || type == EType::SCALAR) {
    m_scalar.~Value();
  }
}

String Types::to_string(const String &prefix) const {
  // TODO duplicate
  String result(prefix);
  if (type == EType::SCALAR) {
    result.append(m_scalar);
    result.append("\n");
  } else if (type == EType::LIST) {
    String nprefix(prefix);
    nprefix.append("  ");

    result.append("\n");
    result.append(m_list.to_string(nprefix));
  } else if (type == EType::MAP) {
    String nprefix(prefix);
    nprefix.append("  ");

    result.append("\n");
    result.append(m_map.to_string(nprefix));
  } else if (type == EType::KV) {
    result.append(m_scalar);
    result.append("\n");
  }
  return result;
}

/*entry*/
entry::entry(entry &&o)
    : m_type(std::move(o.m_type)), m_key(std::move(o.m_key)) {
}

entry::entry(const entry &o) : m_type(o.m_type), m_key(o.m_key) {
}

entry::~entry() {
}

String entry::to_string(const String &prefix) const {
  String result(prefix);
  if (m_type.type == EType::SCALAR) {
    result.append(m_type.m_scalar);
    result.append("\n");
  } else {
    result.append(m_key);
    result.append(": ");
    if (m_type.type == EType::LIST) {
      String nprefix(prefix);
      nprefix.append("  ");

      result.append("\n");
      result.append(m_type.m_list.to_string(nprefix));
    } else if (m_type.type == EType::MAP) {
      String nprefix(prefix);
      nprefix.append("  ");

      result.append("\n");
      result.append(m_type.m_map.to_string(nprefix));
    } else if (m_type.type == EType::KV) {
      result.append(m_type.m_scalar);
      result.append("\n");
    }
  }
  return result;
}
/*private*/

} // namespace yaml
