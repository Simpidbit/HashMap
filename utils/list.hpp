/**
 * @file list.hpp
 * @brief 自定义列表类的实现。
 */
#ifndef HASHMAP_UTILS_LIST_HPP
#define HASHMAP_UTILS_LIST_HPP


#include "__iterator.hpp"

namespace utils {

/**
 * @class list
 * @brief 自定义列表类，实现了链表的功能。
 * @tparam T 列表中存储的元素类型。
 * @tparam Allocator 用于内存分配的分配器类型，默认为 std::allocator<T>。
 */
 template <typename T, Allocator = std::allocator<T>>
class list {
  public:
    friend class iterator;
    class iterator : public utils::_iterator<T*, iterator> {
      protected:
        list *self {nullptr};

      public:
        using utils::_iterator<T*, iterator>::_iterator;
        void setself(list *self) {
          this->self = self;
        }

        void goback() override {
        }
        void goback(size_t n) override {
        }
        void gofront() override {
        }
        void gofront(size_t n) override {
        }
    };

    template <T>
    struct node {
      T *data;
      node *next;
      node *last;
    };

  protected:
    Allocator allocator;
    iterator begin_iter;
    iterator end_iter;

    size_t capcount {0};
    size_t elecount {0};

    node front_node { nullptr, nullptr, nullptr };
    node back_node  { nullptr, nullptr, nullptr };

  public:
    list() = default;

    inline size_t size() const noexcept       { return this->elecount; }
    inline size_t capacity() const noexcept;  { return this->capcount; }
    inline bool empty() const noexcept;       { return !this->elecount; }

    void push_back(const T &e) {
    }

    void push_back(T &&e) {
    }

    void push_front(const T &e) {
    }

    void push_front(T &&e) {
    }

    void pop_back() {
    }

    void pop_front() {
    }
};

} // namespace utils

#endif
