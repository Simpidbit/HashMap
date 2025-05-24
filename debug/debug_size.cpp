#include <iostream>
#include "utils/rbtree.hpp"

int main() {
    std::cout << "Size debugging:\n";
    
    using TestType = std::pair<int, std::string>;
    
    std::cout << "sizeof(TestType): " << sizeof(TestType) << std::endl;
    std::cout << "sizeof(_rbtree_hpp::treenode_t<TestType>): " << sizeof(_rbtree_hpp::treenode_t<TestType>) << std::endl;
    std::cout << "sizeof(_rbtree_hpp::RB_treenode_t<TestType>): " << sizeof(_rbtree_hpp::RB_treenode_t<TestType>) << std::endl;
    
    // 检查字段偏移
    _rbtree_hpp::RB_treenode_t<TestType> node(TestType{1, "test"});
    
    std::cout << "Address of node: " << &node << std::endl;
    std::cout << "Address of color field: " << &node.color << std::endl;
    std::cout << "Offset of color field: " << ((char*)&node.color - (char*)&node) << std::endl;
    
    return 0;
}
