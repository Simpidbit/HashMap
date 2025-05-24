#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "=== Simple Find Debug Test ===\n";
    
    HashMap<int, std::string> map;
    
    // Insert one element
    std::cout << "Inserting (1, \"One\")...\n";
    auto result = map.insert(1, "One");
    std::cout << "Insert result: " << (result.second ? "inserted" : "updated") << "\n";
    std::cout << "Map size: " << map.size() << "\n";
    
    // Check what's in the iterator
    std::cout << "\nIterating through map:\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "Found element: (" << it->first << ", \"" << it->second << "\")\n";
    }
    
    // Test find step by step
    std::cout << "\nTesting find(1) step by step:\n";
    auto begin_it = map.begin();
    auto end_it = map.end();
    
    std::cout << "begin() != end(): " << (begin_it != end_it) << "\n";
    
    if (begin_it != end_it) {
        std::cout << "First element in iterator: (" << begin_it->first << ", \"" << begin_it->second << "\")\n";
        std::cout << "begin()->first == 1: " << (begin_it->first == 1) << "\n";
    }
    
    // Now test find
    std::cout << "\nCalling find(1):\n";
    auto find_result = map.find(1);
    std::cout << "find(1) == end(): " << (find_result == end_it) << "\n";
    
    return 0;
}
