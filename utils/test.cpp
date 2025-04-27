#include "mempool.hpp"
#include <exception>

#include <iostream>

int main()
{
  utils::unique_pool<int> pl(1024ULL * 1024 * 1024 * 1024);
  pl.init();

  return 0;
}
