#include "mempool.hpp"
#include "bitmap.hpp"
#include <exception>

#include <vector>

#include <iostream>


int main()
{
  utils::unique_pool<int> pl;
  pl.init();
  int *n1 = pl.allocate(10);
  std::cout << *n1 << std::endl;

  return 0;
}
