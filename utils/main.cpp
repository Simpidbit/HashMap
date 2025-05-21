#include <iostream>
#include <functional>

int main()
{
  std::function<void(int)> f = [] ( int a ) {return;};
  return 0;
}
