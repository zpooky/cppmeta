#ifndef SP_CPP_META_ARRAY_LIST_H
#define SP_CPP_META_ARRAY_LIST_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>

namespace sp {
namespace impl {
enum class Type : bool { STATIC, SHALLOW };

template <typename, size_t>
struct StaticNode;
template <typename, size_t>
struct ShallowNode;

template <typename NodeType, typename T, size_t slab>
static void node_delete(NodeType &node) {
  if (node.next) {
    if (node.next_type == impl::Type::STATIC) {
      auto n = static_cast<StaticNode<T, slab> *>(node.next);
      delete n;
    } else {
      auto n = static_cast<ShallowNode<T, slab> *>(node.next);
      delete n;
    }
  }
} // node_delete()

template <typename T, size_t slab>
struct StaticNode {
  T ptr[slab];
  void *next;
  size_t index;
  impl::Type next_type;

  StaticNode() : next(nullptr), index(0), next_type(impl::Type::STATIC) {
  }

  ~StaticNode() {
    node_delete<StaticNode, T, slab>(*this);
  }
}; // struct StaticNode

template <typename T, size_t slab>
struct ShallowNode {
  T *ptr;
  void *next;
  size_t index;
  size_t length;
  impl::Type next_type;
  bool gc;

  ShallowNode()
      : ptr(nullptr), next(nullptr), index(0), length(0),
        next_type(impl::Type::STATIC), gc(false) {
  }

  ~ShallowNode() {
    node_delete<ShallowNode, T, slab>(*this);
    if (gc) {
      delete[] ptr;
    }
  }
}; // struct ShallowNode
} // namespace sp::impl

template <typename, size_t>
class ArrayListIterator;

template <typename T, size_t slab = 128>
class ArrayList {
private:
  impl::StaticNode<T, slab> *m_node;

public:
  using iterator = ArrayListIterator<T, slab>;
  using const_iterator = ArrayListIterator<T, slab>;
  friend class ArrayListIterator<T, slab>;

  ArrayList() : m_node(nullptr) {
  }

  template <typename Collection>
  ArrayList(const Collection &c) : ArrayList() {
    for (const auto &current : c) {
      push_back(c);
    }
  }

  ~ArrayList() {
    if (m_node) {
      delete m_node;
      m_node = nullptr;
    }
  }

private:
  void *find_free_inc() {
    if (m_node == nullptr) {
      m_node = new impl::StaticNode<T, slab>;
      if (m_node == nullptr) {
        return nullptr;
      }
    }
    impl::Type type = impl::Type::STATIC;
    void *c = m_node;
  start:
    if (type == impl::Type::STATIC) {
      auto current = static_cast<impl::StaticNode<T, slab> *>(c);
      if (current->index == slab) {
        if (current->next == nullptr) {
          current->next = new impl::StaticNode<T, slab>;
          if (current->next == nullptr) {
            return nullptr;
          }
          current->next_type = impl::Type::STATIC;
        }
        c = current->next;
        type = current->next_type;

        goto start;
      }
    } else {
      auto current = static_cast<impl::ShallowNode<T, slab> *>(c);
      if (current->index == current->length) {
        if (current->next == nullptr) {
          current->next = new impl::StaticNode<T, slab>;
          if (current->next == nullptr) {
            return nullptr;
          }
          current->next_type = impl::Type::STATIC;
        }
        c = current->next;
        type = current->next_type;

        goto start;
      }
    }

    void *dest = nullptr;
    if (type == impl::Type::STATIC) {
      auto current = static_cast<impl::StaticNode<T, slab> *>(c);
      current->ptr[current->index].~T();
      dest = current->ptr + current->index;
      current->index++;
    } else {
      auto current = static_cast<impl::ShallowNode<T, slab> *>(c);
      current->ptr[current->index].~T();
      dest = current->ptr + current->index;
      current->index++;
    }
    return dest;
  } // find_free_inc()

public:
  template <typename E>
  bool push_back(E &&data) {
    void *dest = find_free_inc();
    if (dest) {
      new (dest) T(std::forward<E>(data));
      return true;
    }
    return false;
  } // push_back()

  template <typename... E>
  bool emplace_back(E &&... data) {
    void *dest = find_free_inc();
    if (dest) {
      new (dest) T(std::forward<E>(data)...);
      return true;
    }
    return false;
  } // push_back()

  template <typename E>
  void cc() {
  }

  iterator begin() {
    return iterator(*this);
  } // begin()

  iterator end() {
    //   impl::Type type = impl::Type::STATIC;
    //   void *c = m_node;
    //   size_t index = 0;
    // start:
    //   if (c) {
    //     if (type == impl::Type::STATIC) {
    //       auto current = static_cast<impl::StaticNode<T, slab> *>(c);
    //       index = current->index;
    //       if (current->next) {
    //         type = current->next_type;
    //         c = current->next;
    //         index = current->index;
    //         goto start;
    //       }
    //     } else {
    //       auto current = static_cast<impl::ShallowNode<T, slab> *>(c);
    //       index = current->index;
    //       if (current->next) {
    //         type = current->next_type;
    //         c = current->next;
    //         index = current->index;
    //         goto start;
    //       }
    //     }
    //   }
    //   return iterator(c, index, type);
    return iterator(nullptr, 0, impl::Type::STATIC);
  } // end()

}; // class ArrayList

template <typename T, size_t N>
class ArrayListIterator : public std::iterator<std::input_iterator_tag, T> {
  using SelfType = ArrayListIterator<T, N>;
  void *m_iterator;
  size_t m_index;
  impl::Type m_type;

public:
  ArrayListIterator(void *it, size_t index, impl::Type type)
      : m_iterator(it), m_index(index), m_type(type) {
  }

  ArrayListIterator(ArrayList<T, N> &a)
      : ArrayListIterator(a.m_node, 0, impl::Type::STATIC) {
  }

  bool operator==(const SelfType &o) const {
    // printf("%p == %p && %u == %u\n", m_iterator, o.m_iterator, m_index,
    // o.m_index);
    return (m_iterator == o.m_iterator && m_index == o.m_index);
  }

  bool operator!=(const SelfType &o) const {
    return !operator==(o);
  }

  SelfType &operator++() {
    m_index++;
    if (m_type == impl::Type::STATIC) {
      auto node = static_cast<impl::StaticNode<T, N> *>(m_iterator);
      if (m_index == node->index) {
        m_iterator = node->next;
        m_index = 0;
        m_type = node->next_type;
      }
    } else {
      auto node = static_cast<impl::ShallowNode<T, N> *>(m_iterator);
      if (m_index == node->index) {
        m_iterator = node->next;
        m_index = 0;
        m_type = node->next_type;
      }
    }
    return *this;
  }

  SelfType operator+(uint32_t i) const {
    SelfType ret(*this);
    while (i) {
      ret.operator++();
      --i;
    }
    return ret;
  }

  SelfType operator++(int) {
    SelfType ret(*this);
    operator++();
    return ret;
  }

  T &operator*() {
    if (m_type == impl::Type::STATIC) {
      auto node = static_cast<impl::StaticNode<T, N> *>(m_iterator);
      return node->ptr[m_index];
    } else {
      auto node = static_cast<impl::ShallowNode<T, N> *>(m_iterator);
      return node->ptr[m_index];
    }
  }

  const T &operator*() const {
    if (m_type == impl::Type::STATIC) {
      auto node = static_cast<impl::StaticNode<T, N> *>(m_iterator);
      return node->ptr[m_index];
    } else {
      auto node = static_cast<impl::ShallowNode<T, N> *>(m_iterator);
      return node->ptr[m_index];
    }
  }
}; // class ArrayListIterator

} // namespace sp
#endif
