#include <iostream>
#include "vector.hpp"
#include "queue.hpp"

using std::cout;
using std::cin;
using std::endl;

int main()
{
  utils::static_deque<int> dq(5);
  dq.push_back(3);
  dq.push_back(1);
  dq.push_back(4);

  /*
  auto iter = dq.begin();
  iter++;
  iter++;
  iter++;
  iter++;
  cout << *iter << endl;
  */
  for (auto iter = dq.begin() ; iter != dq.end(); iter++) 
    cout << *iter << endl;



  //utils::vector<int> vec{1, 2, 3};
  /*
  utils::vector<int> vec{1, 2, 3};
  vec.push_back(5);
  vec.push_back(5);
  vec.push_back(9);
  */


  /*
  for (auto iter = vec.begin(); iter != vec.end(); iter++) {
    cout << *iter << endl;
  }
  */

  /*
  for (auto e : vec)
    cout << e << endl;
    */
  /*
  for (auto iter = vec.begin() ; iter != vec.end(); iter++) {
    //cout << "iter.number() = " << iter.number() << endl;
    cout << "vec.end().number() = " << vec.end().number() << endl;
    cout << *iter << endl;
  }
  */




  return 0;
}
