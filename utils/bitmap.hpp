#ifndef HASHMAP_UTILS_BITMAP_HPP
#define HASHMAP_UTILS_BITMAP_HPP

#include "__def.hpp"

namespace utils {

struct _byte {
  unsigned char b1 : 1;
  unsigned char b2 : 1;
  unsigned char b3 : 1;
  unsigned char b4 : 1;
  unsigned char b5 : 1;
  unsigned char b6 : 1;
  unsigned char b7 : 1;
  unsigned char b8 : 1;
};


class bitmap {
  protected:
    unsigned char *bits = nullptr;
    ulint bit_siz = 0;

  public:
    bitmap() = delete;
    bitmap(ulint bit_siz) : bit_siz(bit_siz) {
      this->bits = new unsigned char[bit_siz];
    }
};


} // namespace utils


#endif  // HASHMAP_UTILS_BITMAP_HPP
