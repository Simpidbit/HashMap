#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "Testing HashMap range-based for loop...\n";
    
    HashMap<int, int> map;
    
    // Add elements
    for (int i = 0; i < 4; ++i) {
        map[i] = i * 10;
    }
    
    std::cout << "Elements added. Size: " << map.size() << "\n";
    
    std::cout << "Testing range-based for loop...\n";
    for (const auto& pair : map) {
        std::cout << "  " << pair.first << " -> " << pair.second << "\n";
    }
    
    std::cout << "Test completed!\n";
    return 0;
}
