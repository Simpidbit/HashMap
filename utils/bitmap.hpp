#ifndef HASHMAP_UTILS_BITMAP_HPP
#define HASHMAP_UTILS_BITMAP_HPP

#include "__def.hpp"
#include "__errs.hpp"

#include <cstring>

namespace utils {

template <typename Allocator = std::allocator<unsigned char> >
class bitmap {
  protected:
    Allocator allocator;

  public:
    unsigned char *bits = nullptr;
    unsigned char init_pad = 0b0000'0000;
    ulint bit_count = 0;
    ulint bit_byte = 0;

  public:
    bitmap() = default;
    bitmap(unsigned char init_pad) : init_pad(init_pad) {}

    ~bitmap() {
      this->allocator.deallocate(this->bits, this->bit_byte);
    }

    void init(ulint bit_count) {
      this->bit_count = bit_count;
      this->bit_byte = bit_count % 8 ? bit_count / 8 + 1 : bit_count / 8;
      this->bits = this->allocator.allocate(this->bit_byte);
      std::memset(this->bits, this->init_pad, this->bit_byte);
    }

    void set(ulint location, bool value) {
      if (location > this->bit_count)
        throw utils_exception("bitmap::set(): location > bit_count!");

      ulint byte_offset = location / 8;
      ulint bit_offset = location % 8;
      if (value)
        this->bits[byte_offset] |= static_cast<unsigned char>(0b0000'0001) << bit_offset;
      else
        this->bits[byte_offset] &= ~(static_cast<unsigned char>(0b0000'0001) << bit_offset);
    }

    bool get(ulint location) {
      ulint byte_offset = location / 8;
      ulint bit_offset = location % 8;

      return static_cast<bool>(
          this->bits[byte_offset] & (static_cast<unsigned char>(0b0000'0001) << bit_offset)
      );
    }
};


} // namespace utils


#endif  // HASHMAP_UTILS_BITMAP_HPP
