#ifndef HASHMAP_UTILS_LIST_HPP
#define HASHMAP_UTILS_LIST_HPP


#include "__iterator.hpp"

namespace utils {

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

    // TODO

};

} // namespace utils

#endif
