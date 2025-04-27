#ifndef HASHMAP_UTILS_MEMPOOL_HPP
#define HASHMAP_UTILS_MEMPOOL_HPP


#include "__errs.hpp"
#include "__def.hpp"

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
    T *availptr = nullptr;

    ulint blk_piece_count = 1024 * 4;   // 默认: 4096
    ulint used_count = 0;
    ulint avail_count = 0;
    std::thread t;

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
      try {
        this->membegin = new T[this->blk_piece_count];
      } catch (std::bad_alloc &e) {
        throw utils_exception("unique_pool::init(): Too huge memory wanted!");
      }

      this->avail_count = this->blk_piece_count;

      auto lookfor_free_loop = [this]() -> void {
        for (;;) {
        }
      };

      this->t = std::thread(lookfor_free_loop);
      t.detach();
    }

    template <typename ...Args>
    T* allocate(Args ...args) {

      // TODO
      if (!sizeof...(args))
        return new T;
      else 
        return new T(std::forward(args...));
    }

    void deallocate(T *p) {
      // TODO
      delete p;
    }
};


} // namespace utils


#endif  // HASHMAP_UTILS_MEMPOOL_HPP
