#include "hashmap.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>

void test_basic_operations() {
    std::cout << "=== Testing Basic Operations ===\n";
    
    HashMap<int, std::string> map;
    
    // Test insertion
    auto result1 = map.insert(1, "One");
    auto result2 = map.insert(2, "Two");
    auto result3 = map.insert(1, "Updated One"); // Should not insert
    
    std::cout << "Insert results: " << result1.second << ", " << result2.second << ", " << result3.second << "\n";
    std::cout << "Size: " << map.size() << "\n";
    
    // Test access
    std::cout << "map[1] = " << map[1] << "\n";
    map[3] = "Three";
    std::cout << "After map[3] = 'Three', size: " << map.size() << "\n";
    
    // Test find
    auto it = map.find(2);
    if (it != map.end()) {
        std::cout << "Found key 2: " << it->second << "\n";
    }
    
    auto it_missing = map.find(99);
    std::cout << "Find non-existent key: " << (it_missing == map.end() ? "not found" : "found") << "\n";
}

void test_iterator() {
    std::cout << "\n=== Testing Iterators ===\n";
    
    HashMap<int, std::string> map;
    map.insert(1, "One");
    map.insert(2, "Two");
    map.insert(3, "Three");
    map.insert(4, "Four");
    map.insert(5, "Five");
    
    std::cout << "Iterating through map:\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  " << it->first << " -> " << it->second << "\n";
    }
    
    std::cout << "Range-based for loop:\n";
    for (const auto& pair : map) {
        std::cout << "  " << pair.first << " -> " << pair.second << "\n";
    }
}

void test_erase_operations() {
    std::cout << "\n=== Testing Erase Operations ===\n";
    
    HashMap<int, std::string> map;
    for (int i = 1; i <= 5; ++i) {
        map.insert(i, "Value" + std::to_string(i));
    }
    
    std::cout << "Initial size: " << map.size() << "\n";
    
    // Test erase by key
    bool erased = map.erase(3);
    std::cout << "Erase key 3: " << (erased ? "success" : "failed") << ", size: " << map.size() << "\n";
    
    // Test erase by iterator
    auto it = map.find(1);
    if (it != map.end()) {
        auto next_it = map.erase(it);
        std::cout << "Erased by iterator, size: " << map.size() << "\n";
        if (next_it != map.end()) {
            std::cout << "Next element: " << next_it->first << " -> " << next_it->second << "\n";
        }
    }
    
    std::cout << "Final elements:\n";
    for (const auto& pair : map) {
        std::cout << "  " << pair.first << " -> " << pair.second << "\n";
    }
}

void test_expansion() {
    std::cout << "\n=== Testing Expansion ===\n";
   
    HashMap<int, int> map;
    std::cout << "Initial bucket count: " << map.bucket_count() << "\n";
    
    // Insert enough elements to trigger expansion
    for (int i = 0; i < 20; ++i) {
        std::cout << "测试insert" << std::endl;
        std::cout << "i = " << i << ", i * 10 = " << i * 10 << std::endl;
        map.insert(i, i * 10);
        std::cout << "测试完毕" << std::endl;
        if (i == 12) {
            std::cout << "After 13 insertions - bucket count: " << map.bucket_count() << ", load factor: " << map.load_factor() << "\n";
        }
    }
    
    std::cout << "Final bucket count: " << map.bucket_count() << ", size: " << map.size() << "\n";
    
    // Verify all elements are still accessible
    bool all_found = true;
    for (int i = 0; i < 20; ++i) {
        auto it = map.find(i);
        if (it == map.end() || it->second != i * 10) {
            all_found = false;
            break;
        }
    }
    std::cout << "All elements accessible after expansion: " << (all_found ? "YES" : "NO") << "\n";
}

void test_constructors_and_assignment() {
    std::cout << "\n=== Testing Constructors and Assignment ===\n";
    
    // Default constructor
    HashMap<int, std::string> map1;
    map1.insert(1, "One");
    map1.insert(2, "Two");
    
    // Copy constructor
    HashMap<int, std::string> map2(map1);
    std::cout << "Copy constructor - original size: " << map1.size() << ", copy size: " << map2.size() << "\n";
    
    // Move constructor
    HashMap<int, std::string> map3(std::move(map1));
    std::cout << "Move constructor - moved-from size: " << map1.size() << ", moved-to size: " << map3.size() << "\n";
    
    // Initializer list constructor
    HashMap<int, std::string> map4{{10, "Ten"}, {20, "Twenty"}, {30, "Thirty"}};
    std::cout << "Initializer list constructor size: " << map4.size() << "\n";
    
    // Copy assignment
    map1 = map2;
    std::cout << "Copy assignment - target size: " << map1.size() << "\n";
}

void test_stl_compliance() {
    std::cout << "\n=== Testing STL Compliance ===\n";
    
    HashMap<int, std::string> map;
    
    // Test at() method
    map[1] = "One";
    try {
        std::cout << "map.at(1) = " << map.at(1) << "\n";
        std::cout << "map.at(999) = " << map.at(999) << "\n"; // Should throw
    } catch (const std::out_of_range& e) {
        std::cout << "Expected exception caught: " << e.what() << "\n";
    }
    
    // Test contains()
    std::cout << "contains(1): " << map.contains(1) << "\n";
    std::cout << "contains(999): " << map.contains(999) << "\n";
    
    // Test empty() and clear()
    std::cout << "Empty before clear: " << map.empty() << "\n";
    map.clear();
    std::cout << "Empty after clear: " << map.empty() << ", size: " << map.size() << "\n";
    
    // Test max_size and load factor
    std::cout << "Max size: " << map.max_size() << "\n";
}

int main() {
    try {
        test_basic_operations();
        test_iterator();
        test_erase_operations();
        test_expansion();
        test_constructors_and_assignment();
        test_stl_compliance();
        
        std::cout << "\n=== ALL TESTS PASSED SUCCESSFULLY! ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
    
    return 0;
}
