#include "yaml.h"

namespace yaml {

yaml::yaml(const String &prefix) : m_prefix(prefix) {
}

void yaml::push_back(const Key &, const yaml &) {
}

void yaml::push_back(const Key &, const Value &) {
}

String yaml::to_string() const {
  String result;
  return result;
}
}
