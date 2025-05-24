#ifndef XXHASH32_HPP
#define XXHASH32_HPP

#include "__def.hpp"
#include <cstdint>
#include <stdexcept>

namespace utils {

/**
 * @brief XXHash32 implementation with range mapping support
 * 
 * This class provides a 32-bit XXHash implementation with support for
 * mapping hash values to specified ranges using linear mapping.
 */
class XXHash32 {
private:
    // Constants for XXHash32
    static constexpr uint32_t PRIME32_1 = 2654435761U;
    static constexpr uint32_t PRIME32_2 = 2246822519U;
    static constexpr uint32_t PRIME32_3 = 3266489917U;
    static constexpr uint32_t PRIME32_4 = 668265263U;
    static constexpr uint32_t PRIME32_5 = 374761393U;

    /**
     * @brief Rotate left operation
     */
    static inline uint32_t rotl32(uint32_t x, int r) {
        return (x << r) | (x >> (32 - r));
    }

    /**
     * @brief Core hash computation
     */
    static uint32_t compute_hash(const void* input, size_t length, uint32_t seed = 0) {
        const uint8_t* data = static_cast<const uint8_t*>(input);
        uint32_t h32;
        size_t index = 0;

        if (length >= 16) {
            uint32_t v1 = seed + PRIME32_1 + PRIME32_2;
            uint32_t v2 = seed + PRIME32_2;
            uint32_t v3 = seed + 0;
            uint32_t v4 = seed - PRIME32_1;

            while (index + 16 <= length) {
                v1 = rotl32(v1 + (*reinterpret_cast<const uint32_t*>(data + index)) * PRIME32_2, 13) * PRIME32_1;
                v2 = rotl32(v2 + (*reinterpret_cast<const uint32_t*>(data + index + 4)) * PRIME32_2, 13) * PRIME32_1;
                v3 = rotl32(v3 + (*reinterpret_cast<const uint32_t*>(data + index + 8)) * PRIME32_2, 13) * PRIME32_1;
                v4 = rotl32(v4 + (*reinterpret_cast<const uint32_t*>(data + index + 12)) * PRIME32_2, 13) * PRIME32_1;
                index += 16;
            }

            h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
        } else {
            h32 = seed + PRIME32_5;
        }

        h32 += static_cast<uint32_t>(length);

        while (index + 4 <= length) {
            h32 += (*reinterpret_cast<const uint32_t*>(data + index)) * PRIME32_3;
            h32 = rotl32(h32, 17) * PRIME32_4;
            index += 4;
        }

        while (index < length) {
            h32 += data[index] * PRIME32_5;
            h32 = rotl32(h32, 11) * PRIME32_1;
            index++;
        }

        h32 ^= h32 >> 15;
        h32 *= PRIME32_2;
        h32 ^= h32 >> 13;
        h32 *= PRIME32_3;
        h32 ^= h32 >> 16;

        return h32;
    }

public:
    /**
     * @brief Hash with linear mapping to [min_val, max_val]
     * 
     * Uses linear scaling to maintain uniform distribution across the range.
     * This is the preferred method for bucket mapping in hash tables.
     */
    static uint32_t hash_linear(const void* input, size_t length, 
                               uint32_t min_val, uint32_t max_val, uint32_t seed = 0) {
        if (min_val > max_val) {
            throw std::invalid_argument("min_val must be less than or equal to max_val");
        }
        
        uint32_t hash = compute_hash(input, length, seed);
        uint64_t range = static_cast<uint64_t>(max_val - min_val) + 1;
        
        // Linear scaling: (hash * range) / 2^32 + min_val
        return static_cast<uint32_t>((static_cast<uint64_t>(hash) * range) >> 32) + min_val;
    }

    /**
     * @brief Hash with modulo mapping to [min_val, max_val]
     * 
     * Simple modulo-based mapping. May have slight distribution bias.
     */
    static uint32_t hash_modulo(const void* input, size_t length,
                               uint32_t min_val, uint32_t max_val, uint32_t seed = 0) {
        if (min_val > max_val) {
            throw std::invalid_argument("min_val must be less than or equal to max_val");
        }
        
        uint32_t hash = compute_hash(input, length, seed);
        uint32_t range = max_val - min_val + 1;
        return (hash % range) + min_val;
    }

    /**
     * @brief Get raw hash value without range mapping
     */
    static uint32_t hash_raw(const void* input, size_t length, uint32_t seed = 0) {
        return compute_hash(input, length, seed);
    }
};

} // namespace utils

#endif // XXHASH32_HPP
