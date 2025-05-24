#include "hashmap.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "Basic STL test starting...\n";
    
    try {
        // Test 1: Default constructor
        std::cout << "Creating default HashMap...\n";
        HashMap<int, std::string> map1;
        std::cout << "Default constructor: size = " << map1.size() << "\n";
        
        // Test 2: Simple insert
        std::cout << "Testing simple insert...\n";
        auto result = map1.insert(1, "hello");
        std::cout << "Insert result: inserted = " << result.second << "\n";
        std::cout << "Size after insert: " << map1.size() << "\n";
        
        // Test 3: Access with operator[]
        std::cout << "Testing operator[]...\n";
        map1[2] = "world";
        std::cout << "Size after operator[]: " << map1.size() << "\n";
        
        // Test 4: Find
        std::cout << "Testing find...\n";
        auto it = map1.find(1);
        if (it != map1.end()) {
            std::cout << "Found key 1\n";
        } else {
            std::cout << "Key 1 not found\n";
        }
        
        std::cout << "Basic test completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught\n";
        return 1;
    }
    
    return 0;
}
