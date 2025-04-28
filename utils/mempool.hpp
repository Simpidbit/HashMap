#ifndef HASHMAP_UTILS_MEMPOOL_HPP
#define HASHMAP_UTILS_MEMPOOL_HPP


#include "__errs.hpp"
#include "__def.hpp"
#include "bitmap.hpp"

#include <thread>


namespace utils {


template <typename T>
class _abs_allocator {
  public:
    using value_type = T;

  public:
    virtual T* allocate(ulint n) = 0;
    virtual void deallocate(T *p, ulint n) = 0;
};

template <typename T>
class unique_pool {
  protected:
    T *membegin = nullptr;
    T *memend = nullptr;
    T *availptr = nullptr;
    T *tailptr = nullptr;
    T *last_availptr = nullptr;

    ulint blk_piece_count = 1024 * 4;   // 默认: 4096
    ulint used_count = 0;
    ulint avail_count = 0;
    std::thread search_thread;

    bitmap<> blkmap {0b1111'1111};

  public:
    unique_pool(bool is_init = true) {
      if (is_init)
        this->init();
    }

    unique_pool(ulint n, bool is_init = true) : blk_piece_count(n) {
      if (is_init)
        this->init();
    }

    ~unique_pool() {
      delete[] static_cast<T *>(this->membegin);
    }

    void init() {
      this->blkmap.init(this->blk_piece_count);
      try {
        this->membegin = new T[this->blk_piece_count];
        this->memend = this->membegin + this->blk_piece_count;
        this->tailptr = this->membegin;
        this->availptr = this->membegin;
        this->last_availptr = this->availptr;
      } catch (std::bad_alloc &e) {
        throw utils_exception("unique_pool::init(): Too huge memory wanted!");
      }

      this->avail_count = this->blk_piece_count;

      auto search_loop = [this]() -> void {
        for (;;) {
          if (!this->availptr) {
            for (ulint byte_offset = this->last_availptr - this->membegin;
                byte_offset < this->blk_piece_count; byte_offset++) {
              if (this->blkmap.bits[byte_offset]) {
                ulint bit_offset = 0;
                for (ulint bit_offset = 0; bit_offset < 8; bit_offset++) {
                  if (
                      (static_cast<unsigned char>(0b0000'0001) << bit_offset)
                      & this->blkmap.bits[byte_offset]) {
                    this->availptr = this->membegin + (byte_offset * sizeof(unsigned char)) + bit_offset;
                  }
                }
              }
            }
          }
        }
      };

      this->search_thread = std::thread(search_loop);
      search_thread.detach();
    }

    template <typename ...Args>
    T* allocate(Args ...args) {
      T* ptr = nullptr;
      for (;;) {
        if (this->availptr) {
          this->last_availptr = this->availptr;
          ptr = this->availptr;
          this->availptr = nullptr;
          break;
        }
      }

      this->blkmap.set(static_cast<ulint>(ptr - this->membegin), false);

      if constexpr (std::is_fundamental_v<T>) {
        *ptr = {args...};
      } else {
        if (!sizeof...(args))
          ptr->T();
        else 
          ptr->T(std::forward(args...));
      }

      return ptr;
    }

    void deallocate(T *p) {
      for (;;) {
        if (this->availptr) {
          this->blkmap.set(static_cast<ulint>(p - this->membegin), true);
          if (p < this->availptr)
            this->availptr = p;
        }
      }
    }
};


} // namespace utils


#endif  // HASHMAP_UTILS_MEMPOOL_HPP
