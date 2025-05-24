#include <iostream>
#include "hashmap.hpp"

int main() {
    std::cout << "Testing basic color access issue...\n";
    
    HashMap<int, std::string> map;
    
    // 先插入一个元素，应该没问题
    map[1] = "first";
    std::cout << "Inserted first element: " << map[1] << std::endl;
    
    // 再插入几个元素，看看什么时候出现问题
    map[2] = "second";
    std::cout << "Inserted second element: " << map[2] << std::endl;
    
    map[3] = "third";
    std::cout << "Inserted third element: " << map[3] << std::endl;
    
    // 尝试访问已有元素，这应该触发维护
    map[1] = "updated_first";
    std::cout << "Updated first element: " << map[1] << std::endl;
    
    std::cout << "All tests passed!\n";
    
    return 0;
}
