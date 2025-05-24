#include "hashmap.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "Testing rehash functionality...\n";
    
    HashMap<int, int> map;
    
    // Add some elements
    std::cout << "Adding 5 elements...\n";
    for (int i = 0; i < 5; ++i) {
        map[i] = i * 10;
        std::cout << "Added " << i << " -> " << i * 10 << "\n";
    }
    
    std::cout << "Map size: " << map.size() << "\n";
    std::cout << "Bucket count: " << map.bucket_count() << "\n";
    
    // Test iterating before rehash
    std::cout << "Elements before rehash:\n";
    for (const auto& pair : map) {
        std::cout << "  " << pair.first << " -> " << pair.second << "\n";
    }
    
    std::cout << "Calling rehash(20)...\n";
    map.rehash(20);
    
    std::cout << "After rehash:\n";
    std::cout << "Map size: " << map.size() << "\n";
    std::cout << "Bucket count: " << map.bucket_count() << "\n";
    
    // Test iterating after rehash
    std::cout << "Elements after rehash:\n";
    for (const auto& pair : map) {
        std::cout << "  " << pair.first << " -> " << pair.second << "\n";
    }
    
    std::cout << "Test completed successfully!\n";
    return 0;
}
