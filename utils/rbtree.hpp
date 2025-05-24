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

  public:
    using _rbtree_hpp::RB_tree_t<T, rb_node<T>>::RB_tree_t;
    using node_type = rb_node<T>;
    using base_type = _rbtree_hpp::RB_tree_t<T, rb_node<T>>;

    void print_tree() {
      dynamic_cast<base_type *>(this)->print_tree();
    }

    // Default constructor
    rbtree() = default;
    
    // Copy constructor - performs deep copy
    rbtree(const rbtree& other) : _size(other._size) {
        copy_tree_from(other);
    }
    
    // Move constructor  
    rbtree(rbtree&& other) noexcept : _size(other._size) {
        this->root = other.root;
        other.root = nullptr;
        other._size = 0;
    }
    
    // Copy assignment operator - performs deep copy
    rbtree& operator=(const rbtree& other) {
        if (this != &other) {
            clear_tree();
            _size = other._size;
            copy_tree_from(other);
        }
        return *this;
    }
    
    // Move assignment operator
    rbtree& operator=(rbtree&& other) noexcept {
        if (this != &other) {
            clear_tree();
            this->root = other.root;
            _size = other._size;
            other.root = nullptr;
            other._size = 0;
        }
        return *this;
    }
    
    // Destructor
    ~rbtree() {
        clear_tree();
    }

    node_type* push(const T &val) {
      node_type *node = this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::push(val);
      if (node) this->_size++; 
      return node;
    }

<<<<<<< HEAD
    void push(T &&val) {
      node_type *node = dynamic_cast<base_type*>(this)->push(val);
=======
    node_type* push(T &&val) {
      node_type *node = this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::push(std::move(val));
>>>>>>> e2b1521d4a82d340e55bd5eb44a7678bf4c3de87
      if (node) this->_size++; 
      return node;
    }

<<<<<<< HEAD
    void remove(const T &val) {
      if (dynamic_cast<base_type *>(this)->remove(val)) this->_size--;
    }

    void remove(T &&val) {
      if (dynamic_cast<base_type *>(this)->remove(val)) this->_size--;
    }

    const T* find(const T& target) {
      T* e = &this->search_value(target)->value;
      return e;
=======
    node_type* search_value(const T &val) {
      return this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::search_value(val);
    }

    bool remove(const T &val) {
      return this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::remove(val);
    }

    bool remove(T &&val) {
      return this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::remove(std::move(val));
>>>>>>> e2b1521d4a82d340e55bd5eb44a7678bf4c3de87
    }

    unsigned long long size() const {
      return this->_size;
    }

<<<<<<< HEAD
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
=======
    // Public access to traversal methods
    void trav_in(typename _rbtree_hpp::RB_tree_t<T, rb_node<T>>::trav_action_t action) const {
      this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::trav_in(action);
    }

    void trav_bfs(typename _rbtree_hpp::RB_tree_t<T, rb_node<T>>::trav_action_t action) const {
      this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::trav_bfs(action);
    }

  private:
    // Helper method to perform deep copy of tree structure
    void copy_tree_from(const rbtree& other) {
        if (!other.root) {
            this->root = nullptr;
            return;
        }
        
        // Use BFS traversal to copy all nodes
        this->root = nullptr;
        other.trav_bfs([this](node_type* node, unsigned int level, _rbtree_hpp::left_or_right_e pos) {
            (void)level; // 抑制未使用参数警告
            (void)pos;   // 抑制未使用参数警告
            if (node) {
                this->push(node->value);
            }
        });
    }
    
    // Helper method to clear the tree
    void clear_tree() {
        if (this->root) {
            this->trav_bfs([](node_type* node, unsigned int level, _rbtree_hpp::left_or_right_e pos) {
                (void)node;  // 抑制未使用参数警告
                (void)level; // 抑制未使用参数警告  
                (void)pos;   // 抑制未使用参数警告
                // The destructor of bintree_t will handle the cleanup
            });
            // Let the base class destructor handle cleanup
            this->root = nullptr;
        }
        this->_size = 0;
>>>>>>> e2b1521d4a82d340e55bd5eb44a7678bf4c3de87
    }
};


} // namespace utils


#endif
