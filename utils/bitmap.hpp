#ifndef HASHMAP_UTILS_BITMAP_HPP
#define HASHMAP_UTILS_BITMAP_HPP

#include "__def.hpp"

namespace utils {

struct _byte {
  /* 低位 */
  unsigned char b1 : 1;
  unsigned char b2 : 1;
  unsigned char b3 : 1;
  unsigned char b4 : 1;
  unsigned char b5 : 1;
  unsigned char b6 : 1;
  unsigned char b7 : 1;
  unsigned char b8 : 1;
  /* 高位 */
};

template <typename Allocator = std::allocator<unsigned char> >
class bitmap {
  protected:
    unsigned char *bits = nullptr;
    ulint bit_count = 0;
    Allocator allocator;

  public:
    bitmap() = delete;
    bitmap(ulint bit_count) : bit_count(bit_count) {
      ulint bit_byte = bit_count % 8 ? bit_count / 8 + 1 : bit_count / 8;

      this->bits = this->allocator.allocate(bit_byte);
    }

    ~bitmap() {
      delete this->bits;
    }

    void set(ulint location, bool value) {
    }

    bool get(ulint location) {
    }
};


} // namespace utils


#endif  // HASHMAP_UTILS_BITMAP_HPP
