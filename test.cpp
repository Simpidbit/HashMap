#include "hashmap.hpp"
#include <iostream>
#include <string>

int main()
{
    HashMap<std::string, int> map;

    map.insert("Hello", 100);
    map.insert("World", 20);

    std::cout << "Hello value:" << map["Hello"] << "OK." << std::endl;
    std::cout << "World value:" << map["World"] << "OK." << std::endl;


    return 0;
}
