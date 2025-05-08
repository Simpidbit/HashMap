#ifndef HASHMAP_UTILS_QUEUE_HPP
#define HASHMAP_UTILS_QUEUE_HPP

#include "__def.hpp"
#include <iostream>

namespace utils {


template <typename T, typename Allocator = std::allocator<T> >
class static_deque {
  protected:
    T *membegin = nullptr;
    T *memback = nullptr;
    T *head = nullptr;
    T *tail = nullptr;
    ulint ccount = 0;
    ulint ecount = 0;
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

    ulint count() noexcept {
      return this->ecount;
    }

    ulint capacity() noexcept {
      return this->ccount;
    }

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

};


} // namespace utils

#endif
