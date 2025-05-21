
#ifndef HASHMAP_UTILS_RBTREE_HPP
#define HASHMAP_UTILS_RBTREE_HPP

#include <functional>
#include <iostream>
#include <list>
#include <deque>

#include <cstdint>
#include <cassert>

namespace _rbtree_hpp{
#include "bintree/tree.hpp"
}

namespace utils {

template <typename T>
using rb_node = _rbtree_hpp::RB_treenode_t<T>;

template <typename T>
class rbtree : private _rbtree_hpp::RB_tree_t<T, rb_node<T> > {
  protected:
    unsigned long long _size = 0;

  public:
    using _rbtree_hpp::RB_tree_t<T, rb_node<T>>::RB_tree_t;
    using node_type = rb_node<T>;

    void push(const T &val) {
      node_type *node = this->push(val);
      if (node) this->_size++; 
    }

    void push(T &&val) {
      node_type *node = this->push(val);
      if (node) this->_size++; 
    }

    void remove(const T &val) {
    }

    void remove(T &&val) {
    }

    unsigned long long size() const {
      return this->_size;
    }
};


} // namespace utils


#endif
