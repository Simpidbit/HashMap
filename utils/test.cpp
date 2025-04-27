#include "mempool.hpp"
#include "bitmap.hpp"
#include <exception>

#include <vector>

#include <iostream>


int main()
{
  utils::unique_pool<int> pl;

  unsigned char c = 0b00001110;
  utils::_byte *b = (utils::_byte *)(void *)&c;

  // 0010 0011 0110 0111
  uint16_t n = 0b0010001101100111;
  b = (utils::_byte *)(void *)&n;
  //uint8_t *y = (uint8_t *)(void *)&n;
  //std::cout << (int)*y << std::endl;

  if (b->b1 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b2 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b3 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b4 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b5 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b6 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b7 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  if (b->b8 & 0b1) std::cout << 1 << std::endl;
  else std::cout << 0 << std::endl;

  return 0;
}
