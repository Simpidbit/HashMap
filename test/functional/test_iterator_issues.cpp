#include "hashmap.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Testing HashMap Iterator Issues ===\n\n";
    
    HashMap<int, std::string> map;
    
    // Test 1: Insert some elements
    std::cout << "1. Inserting elements...\n";
    map.insert(1, "one");
    map.insert(2, "two"); 
    map.insert(3, "three");
    
    std::cout << "Map size: " << map.size() << "\n\n";
    
    // Test 2: Find operation
    std::cout << "2. Testing find operation...\n";
    auto it = map.find(2);
    if (it != map.end()) {
        std::cout << "Found key 2, value: '" << it->second << "'\n";
    } else {
        std::cout << "Key 2 not found\n";
    }
    
    // Test 3: Iterator traversal
    std::cout << "\n3. Testing iterator traversal...\n";
    std::cout << "Elements in map:\n";
    for (auto iter = map.begin(); iter != map.end(); ++iter) {
        std::cout << "  Key: " << iter->first << ", Value: '" << iter->second << "'\n";
    }
    
    // Test 4: Range-based for loop
    std::cout << "\n4. Testing range-based for loop...\n";
    std::cout << "Elements in map (range-based):\n";
    for (const auto& pair : map) {
        std::cout << "  Key: " << pair.first << ", Value: '" << pair.second << "'\n";
    }
    
    // Test 5: Element access
    std::cout << "\n5. Testing element access...\n";
    try {
        std::string value = map.at(1);
        std::cout << "map.at(1) = '" << value << "'\n";
        
        std::string& ref = map[2];
        std::cout << "map[2] = '" << ref << "'\n";
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
    
    std::cout << "\n=== Test completed ===\n";
    
    return 0;
}
