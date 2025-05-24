#include <iostream>
#include "rbtree.hpp"

using std::cout;
using std::cin;
using std::endl;

int main()
{
  utils::rbtree<int> tree;
  tree.push(10);
  tree.push(30);
  tree.push(1);
  tree.push(4);
  tree.push(5);
  tree.push(9);
  tree.print_tree();
  cout << tree.size() << endl;

  for (const auto &e : tree)
    cout << e << ", ";
  cout << endl;

  for (auto iter = --tree.end(); iter != --tree.begin(); iter--) {
    cout << *iter << ", ";
  }
  cout << endl;

  return 0;
}
