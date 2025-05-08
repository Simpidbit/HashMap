#include <iostream>
#include "vector.hpp"

using std::cout;
using std::cin;
using std::endl;

int main()
{
  //utils::vector<int> vec{1, 2, 3};
  utils::vector<int> vec{1, 2, 3};
  vec.push_back(5);
  vec.push_back(5);
  vec.push_back(9);


  /*
  for (auto iter = vec.begin(); iter != vec.end(); iter++) {
    cout << *iter << endl;
  }
  */

  for (auto e : vec)
    cout << e << endl;
  /*
  for (auto iter = vec.begin() ; iter != vec.end(); iter++) {
    //cout << "iter.number() = " << iter.number() << endl;
    cout << "vec.end().number() = " << vec.end().number() << endl;
    cout << *iter << endl;
  }
  */




  return 0;
}
