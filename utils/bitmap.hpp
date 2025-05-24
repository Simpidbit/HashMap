#ifndef HASHMAP_UTILS_BITMAP_HPP
#define HASHMAP_UTILS_BITMAP_HPP

#include "__def.hpp"
#include "__errs.hpp"

#include <cstring>

namespace utils {

/**
 * @brief 位图类.
 * @details 逻辑上，它是一个固定长度的数组，其中数组的元素取值只能为 true 或 false. 实际上，它是一块固定长度的内存，内存中每个字节的每一位都是数组的一个元素.
 */
template <typename Allocator = std::allocator<unsigned char> >
class bitmap {
  protected:
    Allocator allocator;

  public:
    unsigned char *bits = nullptr;
    unsigned char init_pad = 0b00000000;
    ulint bit_count = 0;
    ulint bit_byte = 0;

  public:
    bitmap() = default;
    bitmap(unsigned char init_pad) : init_pad(init_pad) {}

    // Copy constructor
    bitmap(const bitmap& other) : init_pad(other.init_pad), bit_count(other.bit_count), bit_byte(other.bit_byte) {
      if (other.bits && other.bit_byte > 0) {
        this->bits = this->allocator.allocate(this->bit_byte);
        std::memcpy(this->bits, other.bits, this->bit_byte);
      }
    }

    // Move constructor
    bitmap(bitmap&& other) noexcept 
      : bits(other.bits), init_pad(other.init_pad), bit_count(other.bit_count), bit_byte(other.bit_byte) {
      other.bits = nullptr;
      other.bit_count = 0;
      other.bit_byte = 0;
    }

    // Copy assignment operator
    bitmap& operator=(const bitmap& other) {
      if (this != &other) {
        // Deallocate current memory
        if (this->bits) {
          this->allocator.deallocate(this->bits, this->bit_byte);
          this->bits = nullptr;
        }
        
        // Copy from other
        this->init_pad = other.init_pad;
        this->bit_count = other.bit_count;
        this->bit_byte = other.bit_byte;
        
        if (other.bits && other.bit_byte > 0) {
          this->bits = this->allocator.allocate(this->bit_byte);
          std::memcpy(this->bits, other.bits, this->bit_byte);
        }
      }
      return *this;
    }

    // Move assignment operator
    bitmap& operator=(bitmap&& other) noexcept {
      if (this != &other) {
        // Deallocate current memory
        if (this->bits) {
          this->allocator.deallocate(this->bits, this->bit_byte);
        }
        
        // Move from other
        this->bits = other.bits;
        this->init_pad = other.init_pad;
        this->bit_count = other.bit_count;
        this->bit_byte = other.bit_byte;
        
        // Reset other
        other.bits = nullptr;
        other.bit_count = 0;
        other.bit_byte = 0;
      }
      return *this;
    }

    ~bitmap() {
      if (this->bits) {
        this->allocator.deallocate(this->bits, this->bit_byte);
      }
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
        this->bits[byte_offset] |= static_cast<unsigned char>(0b00000001) << bit_offset;
      else
        this->bits[byte_offset] &= ~(static_cast<unsigned char>(0b00000001) << bit_offset);
    }

    bool get(ulint location) const {
      ulint byte_offset = location / 8;
      ulint bit_offset = location % 8;

      return static_cast<bool>(
          this->bits[byte_offset] & (static_cast<unsigned char>(0b00000001) << bit_offset)
      );
    }
};


} // namespace utils


#endif  // HASHMAP_UTILS_BITMAP_HPP
