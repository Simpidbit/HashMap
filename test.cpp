#include <iostream>

template <typename T = double>
class test_t {
public:
  using size_type = T;
  test_t() = default;

  void hello(T t);

private:
  void fuck(T t);
};

template <typename T>
void test_t<T>::hello(T t) {
  size_type i = t;
  this->fuck(t);
  std::cout << i << std::endl;
}

template<typename T>
void test_t<T>::fuck(T t) {
  size_type i = t;
  std::cout << "Fuck" << i << std::endl;
}

int main() {
  test_t<int> t;
  t.hello(10);

  std::cout << sizeof(t) << std::endl;

  return 0;
}
