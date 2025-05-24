#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "Creating first HashMap..." << std::endl;
    HashMap<int, std::string> map1;
    
    std::cout << "Creating second HashMap..." << std::endl;  
    HashMap<int, std::string> map2;
    
    std::cout << "Copying map1 to map2..." << std::endl;
    map2 = map1;  // This should trigger the copy assignment
    
    std::cout << "Exiting main..." << std::endl;
    return 0;  // Destructors called here
}
