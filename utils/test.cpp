#include <iostream>
#include <vector>

#include "queue.hpp"

using std::cout;
using std::cin;
using std::endl;

int main()
{
  utils::static_deque<int> *sdq = new utils::static_deque<int>;

  /*
  int c, tmp;
  for (;;) {
    cin >> c >> tmp;
    if (c == 0) {
      bool ret = sdq.push_back(tmp);
      cout << ret << endl;
    } else if (c == 1) {
      bool ret = sdq.push_front(tmp);
      cout << ret << endl;
    } else if (c == 2) {
      if (tmp == 0)
        sdq.pop_front();
      else
        sdq.pop_back();
    }

    for (auto e : sdq)
      cout << e << ", ";
    cout << endl;
  }


  */
  return 0;
}
