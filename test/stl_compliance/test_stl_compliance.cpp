#include "hashmap.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <stdexcept>

void test_constructors() {
    std::cout << "=== Testing Constructors ===\n";
    
    // Default constructor
    HashMap<int, std::string> map1;
    std::cout << "Default constructor: size = " << map1.size() << "\n";
    
    // Constructor with estimated size
    HashMap<int, std::string> map2(100);
    std::cout << "Estimated size constructor: bucket_count = " << map2.bucket_count() << "\n";
    
    // Initializer list constructor
    HashMap<int, std::string> map3{{1, "one"}, {2, "two"}, {3, "three"}};
    std::cout << "Initializer list constructor: size = " << map3.size() << "\n";
    
    // Copy constructor
    HashMap<int, std::string> map4(map3);
    std::cout << "Copy constructor: size = " << map4.size() << "\n";
    
    // Move constructor
    HashMap<int, std::string> map5(std::move(map4));
    std::cout << "Move constructor: size = " << map5.size() << ", moved from size = " << map4.size() << "\n";
}

void test_insert_and_access() {
    std::cout << "\n=== Testing Insert and Access ===\n";
    
    HashMap<int, std::string> map;
    
    // Test insert with return value
    auto result1 = map.insert(1, "Hello");
    auto result2 = map.insert(1, "World"); // Should not insert
    
    std::cout << "Insert(1, \"Hello\"): inserted = " << result1.second << "\n";
    std::cout << "Insert(1, \"World\"): inserted = " << result2.second << "\n";
    
    // Test operator[]
    map[2] = "Two";
    map[3] = "Three";
    
    std::cout << "After insertions: size = " << map.size() << "\n";
    std::cout << "map[1] = \"" << map[1] << "\"\n";
    std::cout << "map[2] = \"" << map[2] << "\"\n";
    
    // Test at() method
    try {
        std::cout << "map.at(2) = \"" << map.at(2) << "\"\n";
        std::cout << "map.at(999) = \"" << map.at(999) << "\"\n"; // Should throw
    } catch (const std::out_of_range& e) {
        std::cout << "Caught expected exception: " << e.what() << "\n";
    }
    
    // Test contains()
    std::cout << "contains(2): " << (map.contains(2) ? "true" : "false") << "\n";
    std::cout << "contains(999): " << (map.contains(999) ? "true" : "false") << "\n";
}

void test_find_and_erase() {
    std::cout << "\n=== Testing Find and Erase ===\n";
    
    HashMap<int, std::string> map;
    map[1] = "One";
    map[2] = "Two";
    map[3] = "Three";
    map[4] = "Four";
    
    std::cout << "Initial size: " << map.size() << "\n";
    
    // Test find
    auto it = map.find(2);
    if (it != map.end()) {
        std::cout << "Found key 2 with value: \"" << it->second << "\"\n";
    }
    
    // Test erase by key
    bool erased = map.erase(2);
    std::cout << "Erase(2): " << (erased ? "success" : "failed") << "\n";
    std::cout << "Size after erase: " << map.size() << "\n";
    
    // Test erase by iterator
    it = map.find(3);
    if (it != map.end()) {
        map.erase(it);
        std::cout << "Erased by iterator, size: " << map.size() << "\n";
    }
}

void test_iterators() {
    std::cout << "\n=== Testing Iterators ===\n";
    
    HashMap<int, std::string> map;
    map[1] = "One";
    map[2] = "Two";
    map[3] = "Three";
    
    std::cout << "Iterating through map:\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  " << it->first << " -> \"" << it->second << "\"\n";
    }
    
    // Range-based for loop (if iterator works properly)
    std::cout << "Range-based for loop:\n";
    for (const auto& pair : map) {
        std::cout << "  " << pair.first << " -> \"" << pair.second << "\"\n";
    }
}

void test_capacity() {
    std::cout << "\n=== Testing Capacity ===\n";
    
    HashMap<int, int> map;
    
    std::cout << "Initial bucket_count: " << map.bucket_count() << "\n";
    std::cout << "Initial load_factor: " << map.load_factor() << "\n";
    
    // Add elements to trigger expansion
    for (int i = 0; i < 20; ++i) {
        map[i] = i * 10;
    }
    
    std::cout << "After 20 insertions:\n";
    std::cout << "  Size: " << map.size() << "\n";
    std::cout << "  Bucket count: " << map.bucket_count() << "\n";
    std::cout << "  Load factor: " << map.load_factor() << "\n";
}

void test_emplace() {
    std::cout << "\n=== Testing Emplace ===\n";
    
    HashMap<int, std::string> map;
    
    // Test emplace
    auto result = map.emplace(1, "Hello");
    std::cout << "Emplace(1, \"Hello\"): inserted = " << result.second << "\n";
    
    // Test emplace with existing key
    auto result2 = map.emplace(1, "World");
    std::cout << "Emplace(1, \"World\"): inserted = " << result2.second << "\n";
    
    std::cout << "Final size: " << map.size() << "\n";
}

void test_various_insert_overloads() {
    std::cout << "\n=== Testing Various Insert Overloads ===\n";
    
    HashMap<int, std::string> map;
    
    // Insert range
    std::vector<std::pair<int, std::string>> pairs = {{1, "One"}, {2, "Two"}, {3, "Three"}};
    map.insert(pairs.begin(), pairs.end());
    std::cout << "Insert range: size = " << map.size() << "\n";
    
    // Insert initializer list
    map.insert({{4, "Four"}, {5, "Five"}});
    std::cout << "Insert initializer list: size = " << map.size() << "\n";
}

void test_debug_and_misc() {
    std::cout << "\n=== Testing Debug and Miscellaneous ===\n";
    
    HashMap<int, std::string> map;
    map[1] = "One";
    map[2] = "Two";
    
    // Test debug output
    map.debug();
    
    // Test other methods
    std::cout << "Empty: " << (map.empty() ? "true" : "false") << "\n";
    std::cout << "Max size: " << map.max_size() << "\n";
    
    // Test clear
    map.clear();
    std::cout << "After clear - empty: " << (map.empty() ? "true" : "false") << "\n";
    std::cout << "After clear - size: " << map.size() << "\n";
}

int main() {
    std::cout << "STL Compliance Test for HashMap\n";
    std::cout << "==============================\n";
    
    try {
        test_constructors();
        test_insert_and_access();
        test_find_and_erase();
        test_iterators();
        test_capacity();
        test_emplace();
        test_various_insert_overloads();
        test_debug_and_misc();
        
        std::cout << "\n=== All tests completed successfully! ===\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
    
    return 0;
}
