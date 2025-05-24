#include "utils/xxhash32.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "Testing XXHash32 implementation...\n";
    
    try {
        // Test with simple string
        std::string test = "hello";
        std::cout << "Testing with string: " << test << "\n";
        
        uint32_t raw_hash = utils::XXHash32::hash_raw(test.c_str(), test.length());
        std::cout << "Raw hash: " << raw_hash << "\n";
        
        uint32_t linear_hash = utils::XXHash32::hash_linear(test.c_str(), test.length(), 0, 15);
        std::cout << "Linear hash[0-15]: " << linear_hash << "\n";
        
        uint32_t modulo_hash = utils::XXHash32::hash_modulo(test.c_str(), test.length(), 0, 15);
        std::cout << "Modulo hash[0-15]: " << modulo_hash << "\n";
        
        std::cout << "XXHash32 test completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
