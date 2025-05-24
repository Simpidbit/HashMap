#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "Testing HashMap step by step...\n";
    
    HashMap<int, int> map;
    
    // Test basic insertion
    std::cout << "1. Testing insertion...\n";
    map[0] = 0;
    map[1] = 10;
    std::cout << "   Size: " << map.size() << "\n";
    
    // Test access  
    std::cout << "2. Testing access...\n";
    std::cout << "   map[0] = " << map[0] << "\n";
    std::cout << "   map[1] = " << map[1] << "\n";
    
    // Test insertion of a third element that might cause issues
    std::cout << "3. Testing third element...\n";
    map[2] = 20;
    std::cout << "   Size: " << map.size() << "\n";
    std::cout << "   map[2] = " << map[2] << "\n";
    
    // Test iterator begin/end (this might be where the issue is)
    std::cout << "4. Testing iterators...\n";
    auto it = map.begin();
    std::cout << "   Got begin iterator\n";
    auto end_it = map.end();
    std::cout << "   Got end iterator\n";
    
    // Test dereferencing iterator
    if (it != end_it) {
        std::cout << "   First element: " << it->first << " -> " << it->second << "\n";
    }
    
    std::cout << "Test completed!\n";
    return 0;
}
