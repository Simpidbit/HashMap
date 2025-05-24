#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "Testing basic HashMap functionality...\n";
    
    HashMap<int, int> map;
    
    std::cout << "Initial size: " << map.size() << "\n";
    
    std::cout << "Adding first element...\n";
    map[1] = 10;
    std::cout << "Size after first insert: " << map.size() << "\n";
    
    std::cout << "Adding second element...\n";
    map[2] = 20;
    std::cout << "Size after second insert: " << map.size() << "\n";
    
    std::cout << "Test completed!\n";
    return 0;
}
