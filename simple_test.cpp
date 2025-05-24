#include "hashmap.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "Creating HashMap with string values...\n";
    
    try {
        HashMap<int, std::string> map(10);
        std::cout << "HashMap created successfully\n";
        std::cout << "Size: " << map.size() << "\n";
        std::cout << "Bucket count: " << map.bucket_count() << "\n";
        
        std::cout << "Testing string inserts...\n";
        auto result1 = map.insert(1, "Hello");
        std::cout << "Inserted 1, size: " << map.size() << "\n";
        
        auto result2 = map.insert(2, "World");
        std::cout << "Inserted 2, size: " << map.size() << "\n";
        
        std::cout << "Test completed successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "Exiting main...\n";
    return 0;
}
