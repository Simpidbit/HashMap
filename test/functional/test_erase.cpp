#include "hashmap.hpp"
#include <iostream>
#include <string>

void test_erase() {
    std::cout << "=== Testing erase operations ===\n";
    
    HashMap<int, std::string> map;
    
    // Insert some elements
    map.insert(1, "One");
    map.insert(2, "Two");
    map.insert(3, "Three");
    map.insert(4, "Four");
    map.insert(5, "Five");
    
    std::cout << "Initial map state:\n";
    std::cout << "  Size: " << map.size() << "\n";
    
    // Print all elements
    std::cout << "Elements:\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
    
    // Test erase(key)
    std::cout << "\nErasing key 3...\n";
    bool removed = map.erase(3);
    std::cout << "Erase result: " << (removed ? "removed" : "not found") << "\n";
    std::cout << "Size after erase: " << map.size() << "\n";
    
    std::cout << "Elements after erase(key):\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
    
    // Test erase(iterator)
    std::cout << "\nErasing element using iterator (key 1)...\n";
    auto it = map.find(1);
    if (it != map.end()) {
        auto next_it = map.erase(it);
        std::cout << "Next element after erase: ";
        if (next_it != map.end()) {
            std::cout << next_it->first << ": " << next_it->second << "\n";
        } else {
            std::cout << "end()\n";
        }
    }
    
    std::cout << "Size after erase: " << map.size() << "\n";
    std::cout << "Elements after erase(iterator):\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
    
    // Test erase(first, last) - erase a range
    std::cout << "\nErasing range (first two elements)...\n";
    auto first = map.begin();
    auto last = map.begin();
    if (last != map.end()) {
        ++last; // Advance to the second element
        if (last != map.end()) {
            ++last; // Advance to the third element
            auto after_range = map.erase(first, last);
            std::cout << "Element after erased range: ";
            if (after_range != map.end()) {
                std::cout << after_range->first << ": " << after_range->second << "\n";
            } else {
                std::cout << "end()\n";
            }
        }
    }
    
    std::cout << "Size after range erase: " << map.size() << "\n";
    std::cout << "Final elements:\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
}

int main() {
    try {
        test_erase();
        
        std::cout << "\n=== All erase tests completed ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
