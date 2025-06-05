#include "hashmap.hpp"
#include <iostream>
#include <string>

int main()
{
    HashMap<std::string, int> map;

    /*
    map.set_hash_block_maker(
      [](const void * ptr, decltype(map)::size_type size) 
        -> std::pair<const void *, decltype(map)::size_type> 
      {
        const std::string *sptr = static_cast<const std::string *>(ptr);
        return std::make_pair(
          sptr->c_str(),
          static_cast<decltype(map)::size_type>(sptr->size())
        );
      }
    );
    */

    std::string k1 = "Hello";
    int v1 = 100;

    map.insert(k1, v1);
    map.insert("World", 20);
    map["Yes"] = 666;

    std::cout << "Hello value: " << map["Hello"] << " OK." << std::endl;
    std::cout << "World value: " << map["World"] << " OK." << std::endl;
    std::cout << "Yes value: " << map["Yes"] << " OK." << std::endl;


    return 0;
}
