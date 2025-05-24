#include "hashmap.hpp"
#include <iostream>

int main() {
    std::cout << "Testing HashMap expansion...\n";
    
    try {
        HashMap<int, int> map(10);  // Small initial size
        
        std::cout << "Initial state:\n";
        std::cout << "  Bucket count: " << map.bucket_count() << "\n";
        std::cout << "  Load factor threshold: 0.75\n";
        
        // Calculate threshold for triggering expansion
        size_t threshold = static_cast<size_t>(map.bucket_count() * 0.75) + 1;
        std::cout << "  Will expand after inserting " << threshold << " elements\n\n";
        
        // Insert elements up to and beyond the threshold
        for (int i = 0; i < threshold + 5; ++i) {
            std::cout << "Inserting " << i << "...\n";
            
            size_t old_bucket_count = map.bucket_count();
            
            auto result = map.insert(i, i * 10);
            
            std::cout << "  Size: " << map.size() 
                      << ", Load factor: " << map.load_factor()
                      << ", Buckets: " << map.bucket_count();
                      
            if (map.bucket_count() != old_bucket_count) {
                std::cout << " [EXPANDED from " << old_bucket_count << "]";
            }
            std::cout << "\n";
        }
        
        std::cout << "\nFinal state:\n";
        std::cout << "  Size: " << map.size() << "\n";
        std::cout << "  Bucket count: " << map.bucket_count() << "\n";
        std::cout << "  Load factor: " << map.load_factor() << "\n";
        
        std::cout << "\nTest completed successfully\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
