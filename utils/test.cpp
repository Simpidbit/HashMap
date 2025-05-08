#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;

struct A {
  int ma{20};
  int mb{30};
};

template <typename T>
class Test {
  ~Test() = delete;
};

template <typename T>
class Test <T*> {
  public:
    T* ptr;

  public:
    Test() {
      cout << "Test Pointer" << endl;
    }

    T& operator*() {
      return *(this->ptr);
    }
    
    T* operator->() {
      return this->ptr;
    }
};

int main()
{
  /*
  Test<A*> t1;
  A aaa{10, 20};
  t1.ptr = &aaa;
  t1->mb = 30;
  cout << t1->mb << endl;
  */
  int a = 10;

  cout << (a += 3) << endl;

  cout << sizeof(size_t) << endl;

  return 0;
}
