#ifndef HASHMAP_UTILS_QUEUE_HPP
#define HASHMAP_UTILS_QUEUE_HPP

#include "__def.hpp"
#include <iostream>

namespace utils {


template <typename T, typename Allocator = std::allocator<T> >
class static_deque {
  public:
    friend class iterator;
    class iterator : public utils::_iterator<T*, iterator> {
      protected:
        static_deque *self {nullptr};

      public:
        using utils::_iterator<T*, iterator>::_iterator;
        void setself(static_deque *self) {
          this->self = self;
        }

        void goback() override {
          self->circle_backstep(this->ptr);
        }
        void goback(size_t n) override {
          for (size_t i = 0; i != n; i++)
            this->goback();
        }
        void gofront() override {
          self->circle_frontstep(this->ptr);
        }
        void gofront(size_t n) override {
          for (size_t i = 0; i != n; i++)
            this->gofront();
        }
    };

  protected:
    T *membegin = nullptr;  // 内存块首地址
    T *memback = nullptr;   // 内存块末地址后一位置
    T *head = nullptr;      // 逻辑首元素前一逻辑位置
    T *tail = nullptr;      // 逻辑末元素后一逻辑位置
    ulint ccount = 0;   // capacity
    ulint ecount = 0;   // element
    Allocator allocator;
  

  protected:
    void insert_at(T *ptr, const T &element) {
      *ptr = element;
    }

    void insert_at(T *ptr, T &&element) {
      *ptr = std::move(element);
    }

    void circle_backstep(T* &ptr) {
      if (ptr == this->memback) ptr = this->membegin;
      else ptr++;
    }

    void circle_frontstep(T* &ptr) {
      if (ptr == this->membegin) ptr = this->memback;
      else ptr--;
    }

  public:
    static_deque() = default;
    static_deque(ulint ccount) : ccount(ccount) {
      this->membegin = this->allocator.allocate(ccount);
      this->memback = this->membegin + ccount;
      this->head = this->membegin;
      this->tail = this->membegin + 1;
    }

    void init(ulint ccount) {
      this->ccount = ccount;
      this->membegin = this->allocator.allocate(ccount);
      this->memback = this->membegin + ccount;
      this->head = this->membegin;
      this->tail = this->membegin + 1;
    }

    inline ulint count() const noexcept { return this->ecount; }
    inline ulint capacity() const noexcept { return this->ccount; }

    bool push_back(T &&element) {
      if (this->ecount < this->ccount) {
        this->insert_at(this->tail, std::forward<T>(element));
        this->ecount++;

        this->circle_backstep(this->tail);

        return true;
      } else return false;
    }

    bool push_front(T &&element) {
      if (this->ecount < this->ccount) {
        this->insert_at(this->head, std::forward<T>(element));
        this->ecount++;

        this->circle_frontstep(this->head);

        return true;
      } else return false;
    }

    void pop_back() {
      this->circle_frontstep(this->tail);
      this->ecount--;
    }

    void pop_front() {
      this->circle_backstep(this->head);
      this->ecount--;
    }

    T & front() {
      T *headtmp = this->head;
      this->circle_backstep(headtmp);
      return *headtmp;
    }

    T & back() {
      T *tailtmp = this->tail;
      this->circle_frontstep(tailtmp);
      return *tailtmp;
    }

    iterator begin() {
      iterator iter;
      iter.setself(this);
      T *headtmp = this->head;
      this->circle_backstep(headtmp);
      iter.point_to(headtmp);
      return iter;
    }

    iterator end() {
      iterator iter;
      iter.setself(this);
      T *tailtmp = this->tail;
      iter.point_to(tailtmp);
      return iter;
    }
};


} // namespace utils

#endif
