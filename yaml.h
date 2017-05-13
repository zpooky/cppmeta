#ifndef SP_CPP_META_YAML_H
#define SP_CPP_META_YAML_H

#include "String.h"

namespace yaml {

using Key = String;
using Value = String;

class yaml {
private:
  String m_prefix;

public:
  yaml(const String &prefix = "");

  void push_back(const Key &, const yaml &);
  void push_back(const Key &, const Value &);

  String to_string() const;

  template <typename T>
  void push_back(const Key &, const T &);
  void push_back(const Key &, const std::string &) = delete;
};

template <typename T>
void yaml::push_back(const Key &key, const T &data) {
  push_back(key, data.to_yaml());
}
}

#endif
