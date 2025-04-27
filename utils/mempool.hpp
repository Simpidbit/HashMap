#ifndef HASHMAP_FACILITY_UTILS_MEMPOOL_HPP
#define HASHMAP_FACILITY_UTILS_MEMPOOL_HPP


#include "__errs.hpp"
#include "__def.hpp"

/*
namespace utils {
  class mempool {
    protected:
      void *memblk = nullptr;

    public:
      mempool() = default;
      void init();

      template <typename T> T * alloc();
      template <typename T> void free(T *ptr);
  };
}
*/

namespace utils {

class _abs_mempool {
  protected:
    void *membegin = nullptr;

  public:
    _abs_mempool() = default;
    virtual ~_abs_mempool() = 0;

    virtual void init() {};
};
_abs_mempool::~_abs_mempool() {}

template <typename T>
class unique_pool : public _abs_mempool {
  protected:
    ulint blk_piece_count = 1024 * 4;   // 默认: 4KB
    ulint used_count = 0;
    ulint avail_count = 0;

  public:
    using target_type = T;

  public:
    unique_pool() = default;
    unique_pool(ulint n) : blk_piece_count(n) {}
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
    }
};


} // namespace utils


#endif  // HASHMAP_FACILITY_UTILS_MEMPOOL_HPP
