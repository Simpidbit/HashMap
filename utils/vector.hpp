#ifndef HASHMAP_UTILS_VECTOR_HPP
#define HASHMAP_UTILS_VECTOR_HPP

#include "__iterator.hpp"

#include <initializer_list>
#include <cstring>

#include <thread>
#include <atomic>


namespace utils {

template <typename T, typename Allocator = std::allocator<T> >
class vector {
  public:
    class iterator : public utils::_iterator<T*, iterator> {
      public:
        using utils::_iterator<T*, iterator>::_iterator;

        void goback() override {
          this->ptr++;
        }
        void goback(size_t n) override {
          this->ptr += n;
        }
        void gofront() override {
          this->ptr--;
        }
        void gofront(size_t n) override {
          this->ptr -= n;
        }
    };

  protected:
    Allocator allocator;
    iterator begin_iter;
    iterator end_iter;

    T* data {nullptr};
    size_t capcount {0};
    size_t elecount {0};

    std::atomic<bool> new_ok {true};
    std::atomic<bool> signal {false};


  protected:
    void new_data_block(size_t ncap, size_t ocap) {
      T* ndata = this->allocator.allocate(ncap);
      this->begin_iter.point_to(ndata);
      this->end_iter.point_to(ndata);
      if (ocap) {
        memcpy(ndata, this->data, ocap * sizeof(T));
        this->allocator.deallocate(this->data, ocap);
        this->end_iter.goback(ocap);
      }
      this->data = ndata;
      this->capcount = ncap;
    }

    void check_capacity(size_t init_capacity = 2) {
      auto target = [this, init_capacity]() -> void {
        this->new_ok.store(false);
        this->signal.store(true);
        if (this->capcount == this->elecount) {
          // Expand
          if (this->capcount == 0)
            this->new_data_block(init_capacity, 0);
          else
            this->new_data_block(this->capcount * 2, this->capcount);
        }
        this->new_ok.store(true);
      };

      std::thread(target).detach();
      for (;;) {
        if (this->signal.load()) {
          this->signal.store(false);
          break;
        }
      }
    }

  public:
    /////////////////////////
    // CONSTRUCTOR, DESTRUCTOR
    /////////////////////////
    vector() {
      this->check_capacity();
    }

    vector(std::initializer_list<T> list) {
      if (list.size()) {
        this->check_capacity(list.size() * 2);
        for (auto e : list) 
          this->push_back(e);
      } else
        this->check_capacity();
    }

    ~vector() {
      this->allocator.deallocate(this->data, this->capcount);
    }

    void push_back(const T &value) {
      for (;;) if (this->new_ok.load()) break;

      *(this->end_iter) = value;
      this->end_iter++;
      this->elecount++;

      this->check_capacity();
    }

    void push_back(T &&value) {
      for (;;) if (this->new_ok.load()) break;

      *(this->end_iter) = std::move(value);
      this->end_iter++;
      this->elecount++;

      this->check_capacity();
    }


    iterator begin() const {
      for (;;) if (this->new_ok.load()) break;

      return this->begin_iter;
    }

    iterator end() const {
      for (;;) if (this->new_ok.load()) break;

      return this->end_iter;
    }

    T& operator[](size_t index) {
      for (;;) if (this->new_ok.load()) break;
      return *(this->begin_iter + index);
    }

    const T& operator[](size_t index) const {
      for (;;) if (this->new_ok.load()) break;
      return *(this->begin_iter + index);
    }

    inline size_t size() const noexcept {
      return this->elecount;
    }
};

} // namespace utils

#endif  // HASHMAP_UTILS_VECTOR_HPP
