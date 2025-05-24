#include "hashmap.hpp"
#include <iostream>

// Let's create a version with debug output
template<typename Key, typename Value>
void debug_find(HashMap<Key, Value>& map, const Key& key) {
    std::cout << "\n=== Debug find(" << key << ") ===\n";
    
    // Simulate find method step by step
    auto bucket_idx = map.get_bucket_index(key);
    std::cout << "Bucket index for key " << key << ": " << bucket_idx << "\n";
    
    // Try to find node in bucket
    auto& bucket = map.buckets_[bucket_idx];
    std::cout << "Bucket size: " << bucket.size() << "\n";
    
    // Check if node exists in bucket
    // We need to access the private find_in_bucket method, so let's just search manually
    std::cout << "Searching in bucket...\n";
    
    bool found_in_bucket = false;
    // We can't access find_in_bucket, so let's just see if we can find it via iterator
    std::cout << "Creating iterator and searching...\n";
    auto it = map.begin();
    auto end_it = map.end();
    int step = 0;
    while (it != end_it) {
        std::cout << "Step " << step << ": (" << it->first << ", \"" << it->second << "\")\n";
        if (it->first == key) {
            std::cout << "Found match at step " << step << "!\n";
            found_in_bucket = true;
            break;
        }
        ++it;
        ++step;
    }
    
    if (!found_in_bucket) {
        std::cout << "Key not found in iterator traversal\n";
    }
    
    // Now call the actual find method
    std::cout << "Calling actual find method...\n";
    auto find_result = map.find(key);
    std::cout << "find() result == end(): " << (find_result == end_it) << "\n";
}

int main() {
    std::cout << "=== Find Method Debug ===\n";
    
    HashMap<int, std::string> map;
    
    // Insert element
    map.insert(1, "One");
    std::cout << "Inserted (1, \"One\")\n";
    std::cout << "Map size: " << map.size() << "\n";
    
    debug_find(map, 1);
    
    return 0;
}
