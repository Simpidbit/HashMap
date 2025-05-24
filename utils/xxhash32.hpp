#ifndef XXHASH32_HPP
#define XXHASH32_HPP

#include "__def.hpp"
#include <cstdint>
#include <stdexcept>

namespace utils {

/**
 * @brief XXHash32算法实现，支持范围映射
 * 
 * 这个类提供了32位XXHash算法的实现，支持将哈希值映射到指定范围。
 * 使用线性映射方式保证了良好的分布均匀性。
 */
class XXHash32 {
private:
    // XXHash32算法的常量
    static constexpr uint32_t PRIME32_1 = 2654435761U;
    static constexpr uint32_t PRIME32_2 = 2246822519U;
    static constexpr uint32_t PRIME32_3 = 3266489917U;
    static constexpr uint32_t PRIME32_4 = 668265263U;
    static constexpr uint32_t PRIME32_5 = 374761393U;

    /**
     * @brief 左循环移位操作
     * @param x 要移位的值
     * @param r 移位位数
     * @return 循环左移后的值
     */
    static inline uint32_t rotl32(uint32_t x, int r) {
        return (x << r) | (x >> (32 - r));
    }

    /**
     * @brief 核心哈希计算函数
     * @param input 输入数据指针
     * @param length 数据长度
     * @param seed 种子值，默认为0
     * @return 计算得到的32位哈希值
     */
    static uint32_t compute_hash(const void* input, size_t length, uint32_t seed = 0) {
        const uint8_t* data = static_cast<const uint8_t*>(input);
        uint32_t h32;
        size_t index = 0;

        // 如果数据长度>=16字节，使用快速循环处理
        if (length >= 16) {
            // 初始化四个累加器
            uint32_t v1 = seed + PRIME32_1 + PRIME32_2;
            uint32_t v2 = seed + PRIME32_2;
            uint32_t v3 = seed + 0;
            uint32_t v4 = seed - PRIME32_1;

            // 每次处理16字节数据
            while (index + 16 <= length) {
                v1 = rotl32(v1 + (*reinterpret_cast<const uint32_t*>(data + index)) * PRIME32_2, 13) * PRIME32_1;
                v2 = rotl32(v2 + (*reinterpret_cast<const uint32_t*>(data + index + 4)) * PRIME32_2, 13) * PRIME32_1;
                v3 = rotl32(v3 + (*reinterpret_cast<const uint32_t*>(data + index + 8)) * PRIME32_2, 13) * PRIME32_1;
                v4 = rotl32(v4 + (*reinterpret_cast<const uint32_t*>(data + index + 12)) * PRIME32_2, 13) * PRIME32_1;
                index += 16;
            }

            // 合并四个累加器
            h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
        } else {
            // 短数据直接使用种子值
            h32 = seed + PRIME32_5;
        }

        // 添加数据长度
        h32 += static_cast<uint32_t>(length);

        // 处理剩余的4字节块
        while (index + 4 <= length) {
            h32 += (*reinterpret_cast<const uint32_t*>(data + index)) * PRIME32_3;
            h32 = rotl32(h32, 17) * PRIME32_4;
            index += 4;
        }

        // 处理剩余的单个字节
        while (index < length) {
            h32 += data[index] * PRIME32_5;
            h32 = rotl32(h32, 11) * PRIME32_1;
            index++;
        }

        // 最终混合步骤
        h32 ^= h32 >> 15;
        h32 *= PRIME32_2;
        h32 ^= h32 >> 13;
        h32 *= PRIME32_3;
        h32 ^= h32 >> 16;

        return h32;
    }

public:
    /**
     * @brief 线性映射哈希函数，将哈希值映射到[min_val, max_val]范围
     * 
     * 使用线性缩放方法保持在指定范围内的均匀分布。
     * 这是哈希表桶映射的首选方法，比模运算有更好的分布性。
     * 
     * @param input 输入数据指针
     * @param length 数据长度
     * @param min_val 范围最小值
     * @param max_val 范围最大值
     * @param seed 种子值，默认为0
     * @return 映射到指定范围内的哈希值
     * @throws std::invalid_argument 当min_val > max_val时抛出异常
     */
    static uint32_t hash_linear(const void* input, size_t length, 
                               uint32_t min_val, uint32_t max_val, uint32_t seed = 0) {
        if (min_val > max_val) {
            throw std::invalid_argument("min_val must be less than or equal to max_val");
        }
        
        uint32_t hash = compute_hash(input, length, seed);
        uint64_t range = static_cast<uint64_t>(max_val - min_val) + 1;
        
        // 线性缩放公式: (hash * range) / 2^32 + min_val
        return static_cast<uint32_t>((static_cast<uint64_t>(hash) * range) >> 32) + min_val;
    }

    /**
     * @brief 模运算映射哈希函数，将哈希值映射到[min_val, max_val]范围
     * 
     * 使用简单的模运算进行映射，可能存在轻微的分布偏差。
     * 计算速度快，但分布均匀性不如线性映射。
     * 
     * @param input 输入数据指针
     * @param length 数据长度
     * @param min_val 范围最小值
     * @param max_val 范围最大值
     * @param seed 种子值，默认为0
     * @return 映射到指定范围内的哈希值
     * @throws std::invalid_argument 当min_val > max_val时抛出异常
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
     * @brief 获取原始哈希值，不进行范围映射
     * 
     * @param input 输入数据指针
     * @param length 数据长度
     * @param seed 种子值，默认为0
     * @return 原始的32位哈希值
     */
    static uint32_t hash_raw(const void* input, size_t length, uint32_t seed = 0) {
        return compute_hash(input, length, seed);
    }
};

} // namespace utils

#endif // XXHASH32_HPP
