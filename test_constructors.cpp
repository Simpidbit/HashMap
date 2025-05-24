#include "hashmap.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "Testing problematic constructors...\n";
    
    try {
        // Test 1: Basic constructor
        std::cout << "1. Testing basic constructor...\n";
        HashMap<int, std::string> map1;
        std::cout << "   Basic constructor works\n";
        
        // Test 2: Insert some data
        std::cout << "2. Adding some data...\n";
        map1[1] = "one";
        map1[2] = "two";
        std::cout << "   Added data, size = " << map1.size() << "\n";
        
        // Test 3: Copy constructor (this might be the issue)
        std::cout << "3. Testing copy constructor...\n";
        HashMap<int, std::string> map2(map1);
        std::cout << "   Copy constructor works, size = " << map2.size() << "\n";
        
        // Test 4: Initializer list constructor (this might also be an issue)
        std::cout << "4. Testing initializer list constructor...\n";
        HashMap<int, std::string> map3{{1, "one"}};
        std::cout << "   Initializer list constructor works, size = " << map3.size() << "\n";
        
        std::cout << "All tests passed!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught\n";
        return 1;
    }
    
    return 0;
}
