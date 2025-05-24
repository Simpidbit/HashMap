#include "hashmap.hpp"
#include <iostream>
#include <string>
#include <cassert>

void test_basic_operations() {
    std::cout << "Testing basic HashMap operations...\n";
    
    // Test constructor
    HashMap<int, std::string> map(100);  // Estimated size of 100
    
    std::cout << "Initial state:\n";
    std::cout << "  Size: " << map.size() << "\n";
    std::cout << "  Empty: " << (map.empty() ? "true" : "false") << "\n";
    std::cout << "  Bucket count: " << map.bucket_count() << "\n";
    std::cout << "  Load factor: " << map.load_factor() << "\n";
    
    // Test insertion
    std::cout << "\nTesting insertion...\n";
    auto result1 = map.insert(1, "Hello");
    auto result2 = map.insert(2, "World");
    auto result3 = map.insert(1, "Duplicate");  // Should not insert
    
    std::cout << "After insertions:\n";
    std::cout << "  Size: " << map.size() << "\n";
    std::cout << "  Load factor: " << map.load_factor() << "\n";
    
    // Test find operation
    std::cout << "\nTesting find operations...\n";
    auto it1 = map.find(1);
    auto it2 = map.find(2);
    auto it3 = map.find(999);  // Should not be found
    
    std::cout << "Find(1): " << (it1 != map.end() ? "found" : "not found") << "\n";
    std::cout << "Find(2): " << (it2 != map.end() ? "found" : "not found") << "\n";
    std::cout << "Find(999): " << (it3 != map.end() ? "found" : "not found") << "\n";
    
    // Test clear
    std::cout << "\nTesting clear...\n";
    map.clear();
    std::cout << "After clear:\n";
    std::cout << "  Size: " << map.size() << "\n";
    std::cout << "  Empty: " << (map.empty() ? "true" : "false") << "\n";
}

void test_expansion() {
    std::cout << "\n=== Testing bucket expansion ===\n";
    
    HashMap<int, int> map(10);  // Small initial size to trigger expansion
    
    std::cout << "Initial bucket count: " << map.bucket_count() << "\n";
    
    // Insert many elements to trigger expansion
    for (int i = 0; i < 20; ++i) {
        map.insert(i, i * i);
        std::cout << "Inserted " << i << ", size=" << map.size() 
                  << ", load_factor=" << map.load_factor()
                  << ", buckets=" << map.bucket_count() << "\n";
    }
    
    std::cout << "Final state:\n";
    std::cout << "  Size: " << map.size() << "\n";
    std::cout << "  Bucket count: " << map.bucket_count() << "\n";
    std::cout << "  Load factor: " << map.load_factor() << "\n";
}

void test_hash_distribution() {
    std::cout << "\n=== Testing hash distribution ===\n";
    
    using utils::XXHash32;
    
    // Test hash function with different inputs
    std::string test_keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    
    std::cout << "Hash values for different strings:\n";
    for (const auto& key : test_keys) {
        uint32_t raw_hash = XXHash32::hash_raw(key.c_str(), key.length());
        uint32_t linear_hash = XXHash32::hash_linear(key.c_str(), key.length(), 0, 15);
        uint32_t modulo_hash = XXHash32::hash_modulo(key.c_str(), key.length(), 0, 15);
        
        std::cout << "  \"" << key << "\":\n";
        std::cout << "    Raw: " << raw_hash << "\n";
        std::cout << "    Linear[0-15]: " << linear_hash << "\n";
        std::cout << "    Modulo[0-15]: " << modulo_hash << "\n";
    }
}

int main() {
    std::cout << "=== HashMap Implementation Test ===\n\n";
    
    try {
        test_basic_operations();
        test_expansion();
        
        std::cout << "\n=== All tests completed ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
