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

    node_type* push(T &&val) {
      node_type *node = this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::push(std::move(val));
      if (node) this->_size++; 
      return node;
    }

    node_type* search_value(const T &val) {
      return this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::search_value(val);
    }

    bool remove(const T &val) {
      return this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::remove(val);
    }

    bool remove(T &&val) {
      return this->_rbtree_hpp::RB_tree_t<T, rb_node<T>>::remove(std::move(val));
    }

    unsigned long long size() const {
      return this->_size;
    }

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
    }
};


} // namespace utils


#endif
