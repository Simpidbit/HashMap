#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "=== HashMap Iterator Debug Test ===\n";
    
    HashMap<int, std::string> map;
    
    // Insert some elements
    std::cout << "Inserting elements...\n";
    auto result1 = map.insert(1, "One");
    auto result2 = map.insert(2, "Two");
    auto result3 = map.insert(3, "Three");
    
    std::cout << "Insert results:\n";
    std::cout << "  (1, \"One\"): " << (result1.second ? "inserted" : "updated") << "\n";
    std::cout << "  (2, \"Two\"): " << (result2.second ? "inserted" : "updated") << "\n";
    std::cout << "  (3, \"Three\"): " << (result3.second ? "inserted" : "updated") << "\n";
    
    std::cout << "\nHashMap state:\n";
    std::cout << "  Size: " << map.size() << "\n";
    std::cout << "  Empty: " << (map.empty() ? "true" : "false") << "\n";
    
    // Test iterator traversal
    std::cout << "\nIterating through all elements:\n";
    int count = 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "  Element " << count++ << ": (" << it->first << ", \"" << it->second << "\")\n";
    }
    
    // Test find operations
    std::cout << "\nTesting find operations:\n";
    auto it1 = map.find(1);
    auto it2 = map.find(2);
    auto it3 = map.find(3);
    auto it4 = map.find(999);
    
    std::cout << "Find(1): " << (it1 != map.end() ? "found" : "not found");
    if (it1 != map.end()) {
        std::cout << " -> (" << it1->first << ", \"" << it1->second << "\")";
    }
    std::cout << "\n";
    
    std::cout << "Find(2): " << (it2 != map.end() ? "found" : "not found");
    if (it2 != map.end()) {
        std::cout << " -> (" << it2->first << ", \"" << it2->second << "\")";
    }
    std::cout << "\n";
    
    std::cout << "Find(3): " << (it3 != map.end() ? "found" : "not found");
    if (it3 != map.end()) {
        std::cout << " -> (" << it3->first << ", \"" << it3->second << "\")";
    }
    std::cout << "\n";
    
    std::cout << "Find(999): " << (it4 != map.end() ? "found" : "not found") << "\n";
    
    return 0;
}
