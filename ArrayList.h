#ifndef SP_CPP_META_ARRAY_LIST_H
#define SP_CPP_META_ARRAY_LIST_H

#include <cstddef>
#include <utility>

namespace sp {

template <typename T, size_t slab = 128>
class ArrayList {
private:
  struct StaticNode;
  struct ShallowNode;

  enum class Type : bool { STATIC, SHALLOW };

  template <typename NodeType>
  static void node_delete(NodeType &node) {
    if (node.next) {
      if (node.next_type == Type::STATIC) {
        ShallowNode *n = node.next;
        delete n;
      } else {
        StaticNode *n = node.next;
        delete n;
      }
    }
  }

  struct StaticNode {
    T ptr[slab];
    void *next;
    size_t index;
    Type next_type;

    StaticNode() : next(nullptr), index(0), next_type(Type::STATIC) {
    }

    ~StaticNode() {
      node_delete(*this);
    }
  }; // struct StaticNode

  struct ShallowNode {
    T *ptr;
    void *next;
    size_t index;
    size_t length;
    Type next_type;
    bool gc;
    ShallowNode()
        : ptr(nullptr), next(nullptr), index(0), length(0),
          next_type(Type::STATIC), gc(false) {
    }

    ~ShallowNode() {
      node_delete(*this);
      if(gc){
        delete[]ptr;
      }
    }
  }; // struct ShallowNode

  StaticNode *m_node;

public:
  ArrayList() : m_node(nullptr) {
  }

  template <typename E>
  bool push_back(E &&data) {
    if (m_node == nullptr) {
      m_node = new StaticNode;
    }
    Type type = Type::STATIC;
    void *c = m_node;
  start:
    if (type == Type::STATIC) {
      StaticNode *current = c;
      if (current->index == slab) {
        if (current->next == nullptr) {
          current->next = new StaticNode;
          if (current->next == nullptr) {
            return false;
          }
          current->next_type = Type::STATIC;
        }
        c = current->next;
        type = current->next_type;

        goto start;
      }
    } else {
      ShallowNode *current = c;
      if (current->index == slab) {
        if (current->next == nullptr) {
          current->next = new StaticNode;
          if (current->next == nullptr) {
            return false;
          }
          current->next_type = Type::STATIC;
        }
        c = current->next;
        type = current->next_type;

        goto start;
      }

      void *dest = nullptr;
      if (type == Type::STATIC) {
        StaticNode *current = c;
        current->ptr[current->index].~T();
        dest = current->ptr + current->index;
        current->index++;
      } else {
        ShallowNode *current;
        current->ptr[current->index].~T();
        dest = current->ptr + current->index;
        current->index++;
      }
      new (dest) T(std::forward<E>(data));
      return true;
    }
  } // push_back

  template <typename E>
  void cc() {
  }
}; // class ArrayList
} // namespace sp
#endif
