#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "Testing HashMap with loop insertion...\n";
    
    HashMap<int, int> map;
    
    // Test loop insertion step by step
    for (int i = 0; i < 10; ++i) {
        std::cout << "Adding element " << i << "...\n";
        map[i] = i * 10;
        std::cout << "  Size: " << map.size() << "\n";
        
        // Test accessing just inserted element
        std::cout << "  map[" << i << "] = " << map[i] << "\n";
        
        if (i >= 3) {
            std::cout << "  Breaking at i=3 to avoid too much output\n";
            break;
        }
    }
    
    std::cout << "Final size: " << map.size() << "\n";
    std::cout << "Test completed!\n";
    return 0;
}
