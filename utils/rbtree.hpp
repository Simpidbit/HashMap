
#ifndef HASHMAP_UTILS_RBTREE_HPP
#define HASHMAP_UTILS_RBTREE_HPP

#include "__iterator.hpp"

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
class rbtree : protected _rbtree_hpp::RB_tree_t<T, rb_node<T> > {
  public:
    class iterator : public utils::_iterator<T*, iterator> {
      friend class rbtree;

      public:
        bool is_back () {}
        bool is_begin () {}

      protected:
        bool is_end {false};
        bool is_begin_front {false};


        rb_node<T> *root;

        rb_node<T> *get_node() {
          return reinterpret_cast<rb_node<T> *>(
              reinterpret_cast<char *>(this->ptr) - reinterpret_cast<char *>(&(static_cast<rb_node<T> *>(0)->value))
          );
        }

      public:
        using utils::_iterator<T*, iterator>::_iterator;

        bool operator==(const iterator &iter) {
          if (this->ptr == iter.ptr) {
            if (this->is_end != iter.is_end)
              return false;
            else if (this->is_begin_front != iter.is_begin_front)
              return false;
            else return true;
          } else return false;
        }

        bool operator!=(const iterator &iter) {
          if (this->is_end && iter.is_end) return false;
          if (this->is_begin_front && iter.is_begin_front) return false;
          else return !(*this == iter);
        }

        iterator(rb_node<T> *root) : root(root) {}

        void point_to(T* nptr) override {
          this->ptr = nptr;
          /* ... */
        }

        void goback() override {
          if (this->is_end) return;

          if (this->is_begin_front) {
            this->is_begin_front = false;
            return;
          }

          rb_node<T> *node = this->get_node();

          // 寻找后继
          if (node->right()) {    // 有右子
            node = node->right();
            while (node->left()) node = node->left();
          } else {                // 无右子
            if (node == this->root) {                       // 是无右子的根节点
              this->is_end = true;
            } else if (node == node->parent()->left()) {    // 是父的左子
              node = node->parent();
            } else {                                        // 是父的右子
              // 往上找
              for (;;) {
                node = node->parent();
                if (node == this->root) {
                  is_end = true;
                  break;
                }

                if (node == node->parent()->left()) {
                  node = node->parent();
                  break;
                }
              }
            }
          }
          if (!is_end) this->ptr = &node->value;
        }

        void goback(size_t n) override {
          for (size_t i = 0; i != n; i++) this->goback();
        }

        void gofront() override {
          if (this->is_begin_front) return;
          if (this->is_end) {
            this->is_end = false;
            return;
          }

          rb_node<T> *node = this->get_node();

          // 寻找前驱
          if (node->left()) {     // 有左子
            node = node->left();
            while (node->right()) node = node->right();
          } else {                // 无左子
            if (node == this->root) {
              this->is_begin_front = true;
            } else if (node == node->parent()->right()) {
              node = node->parent();
            } else {
              // 往上找
              for (;;) {
                node = node->parent();
                if (node == this->root) {
                  is_begin_front = true;
                  break;
                }

                if (node == node->parent()->right()) {
                  node = node->parent();
                  break;
                }
              }
            }
          }
          this->ptr = &node->value;
        }

        void gofront(size_t n) override {
          for (size_t i = 0; i != n; i++) this->gofront();
        }
    };

  protected:
    unsigned long long _size = 0;
    using node_type = rb_node<T>;
    using base_type = _rbtree_hpp::RB_tree_t<T, rb_node<T>>;

  public:
    using _rbtree_hpp::RB_tree_t<T, rb_node<T>>::RB_tree_t;

    void print_tree() {
      dynamic_cast<base_type *>(this)->print_tree();
    }

    void push(const T &val) {
      node_type *node = this->push(val);
      if (node) this->_size++; 
    }

    void push(T &&val) {
      node_type *node = dynamic_cast<base_type*>(this)->push(val);
      if (node) this->_size++; 
    }

    void remove(const T &val) {
      if (dynamic_cast<base_type *>(this)->remove(val)) this->_size--;
    }

    void remove(T &&val) {
      if (dynamic_cast<base_type *>(this)->remove(val)) this->_size--;
    }

    const T* find(const T& target) {
      T* e = &this->search_value(target)->value;
      return e;
    }

    unsigned long long size() const {
      return this->_size;
    }

    iterator begin() const {
      iterator iter(this->root);
      node_type *node = this->root;
      while (node->left()) node = node->left();
      iter.point_to(&node->value);
      return iter;
    }

    iterator end() const {
      iterator iter(this->root);
      node_type *node = this->root;
      while (node->right()) node = node->right();
      iter.point_to(&node->value);
      iter.is_end = true;
      return iter;
    }
};


} // namespace utils


#endif
