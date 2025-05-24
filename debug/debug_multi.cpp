#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "=== Multi-Element Find Debug ===\n";
    
    HashMap<int, std::string> map;
    
    // Insert and test each element one by one
    std::cout << "\n--- Testing single element ---\n";
    map.insert(1, "One");
    auto find1 = map.find(1);
    std::cout << "After inserting (1, \"One\"): find(1) " 
              << (find1 != map.end() ? "found" : "not found") << "\n";
    
    std::cout << "\n--- Testing two elements ---\n";
    map.insert(2, "Two");
    find1 = map.find(1);
    auto find2 = map.find(2);
    std::cout << "After inserting (2, \"Two\"): find(1) " 
              << (find1 != map.end() ? "found" : "not found") << "\n";
    std::cout << "After inserting (2, \"Two\"): find(2) " 
              << (find2 != map.end() ? "found" : "not found") << "\n";
    
    std::cout << "\n--- Testing three elements ---\n";
    map.insert(3, "Three");
    find1 = map.find(1);
    find2 = map.find(2);
    auto find3 = map.find(3);
    std::cout << "After inserting (3, \"Three\"): find(1) " 
              << (find1 != map.end() ? "found" : "not found") << "\n";
    std::cout << "After inserting (3, \"Three\"): find(2) " 
              << (find2 != map.end() ? "found" : "not found") << "\n";
    std::cout << "After inserting (3, \"Three\"): find(3) " 
              << (find3 != map.end() ? "found" : "not found") << "\n";
    
    std::cout << "\n--- Iterator content ---\n";
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "Iterator shows: (" << it->first << ", \"" << it->second << "\")\n";
    }
    
    return 0;
}
