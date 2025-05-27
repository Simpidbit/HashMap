#include "hashmap.hpp"
#include <iostream>
#include <string>
#include <chrono>

// 简化的基准测试函数
template<typename Func>
double benchmark(const std::string& name, Func&& func, int iterations = 1000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        func(i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double time_ms = duration.count() / 1000.0;
    std::cout << name << ": " << time_ms << " 毫秒 (" << iterations << " 次操作)\n";
    return time_ms;
}

int main() {
    std::cout << "=== C++ STL 就地构造概念演示 ===\n\n";
    
    const int test_size = 10000;
    
    // 测试1: 传统插入 vs emplace (使用string类型)
    std::cout << "【测试1: 传统插入 vs emplace (字符串类型)】\n";
    {
        HashMap<int, std::string> map1, map2;
        
        // 传统插入方式
        double time1 = benchmark("传统插入", [&](int i) {
            map1.insert(i, "value_" + std::to_string(i));
        }, test_size);
        
        // emplace方式
        double time2 = benchmark("emplace插入", [&](int i) {
            map2.emplace(i, "value_" + std::to_string(i));
        }, test_size);
        
        double improvement = ((time1 - time2) / time1) * 100;
        std::cout << "性能差异: " << improvement << "%\n";
        std::cout << "map1 大小: " << map1.size() << ", map2 大小: " << map2.size() << "\n\n";
    }
    
    // 测试2: 重复插入性能测试
    std::cout << "【测试2: 重复插入性能测试】\n";
    {
        HashMap<int, std::string> map1, map2;
        
        // 先填充一些数据
        for (int i = 0; i < test_size / 2; ++i) {
            map1.insert(i, "existing" + std::to_string(i));
            map2.emplace(i, "existing" + std::to_string(i));
        }
        
        // 测试插入重复键的性能
        double time1 = benchmark("重复insert", [&](int i) {
            map1.insert(i % (test_size / 2), "updated" + std::to_string(i));
        }, test_size);
        
        double time2 = benchmark("重复emplace", [&](int i) {
            map2.emplace(i % (test_size / 2), "updated" + std::to_string(i));
        }, test_size);
        
        double diff = ((time1 - time2) / time1) * 100;
        std::cout << "性能差异: " << diff << "%\n";
        std::cout << "map1 大小: " << map1.size() << ", map2 大小: " << map2.size() << "\n\n";
    }
    
    // 测试3: 不同数据类型的emplace性能
    std::cout << "【测试3: 不同数据类型emplace性能】\n";
    {
        HashMap<int, int> int_map;
        HashMap<int, std::string> string_map;
        
        // 整数类型
        double time1 = benchmark("int emplace", [&](int i) {
            int_map.emplace(i, i * 2);
        }, test_size);
        
        // 字符串类型  
        double time2 = benchmark("string emplace", [&](int i) {
            string_map.emplace(i, "value" + std::to_string(i));
        }, test_size);
        
        std::cout << "int map 大小: " << int_map.size() << ", string map 大小: " << string_map.size() << "\n\n";
    }
    
    // 测试4: 功能验证
    std::cout << "【测试4: 功能验证】\n";
    {
        HashMap<int, std::string> map;
        
        // 普通插入
        auto [iter1, success1] = map.insert(1, "Hello");
        std::cout << "insert(1, \"Hello\"): " << (success1 ? "成功" : "失败") << "\n";
        
        // emplace
        auto [iter2, success2] = map.emplace(2, "World");
        std::cout << "emplace(2, \"World\"): " << (success2 ? "成功" : "失败") << "\n";
        
        // 第三个元素
        auto [iter3, success3] = map.insert(3, "Additional");
        std::cout << "insert(3, \"Additional\"): " << (success3 ? "成功" : "失败") << "\n";
        
        std::cout << "最终map大小: " << map.size() << "\n";
        std::cout << "内容:\n";
        for (const auto& pair : map) {
            std::cout << "  " << pair.first << " -> \"" << pair.second << "\"\n";
        }
        std::cout << std::endl;
    }
    
    // 概念总结
    std::cout << "=== 概念总结 ===\n";
    std::cout << "1. 【就地构造的优势】:\n";
    std::cout << "   - emplace(args...)直接构造对象\n";
    std::cout << "   - 避免临时对象的创建\n";
    std::cout << "   - 使用完美转发: std::forward<Args>(args)...\n";
    std::cout << "   - 对复杂对象效果更明显\n\n";
    
    std::cout << "2. 【HashMap实现详情】:\n";
    std::cout << "   - emplace通过构造pair然后移动实现\n";
    std::cout << "   - 简化的接口，专注核心功能\n";
    std::cout << "   - 保持STL兼容的接口设计\n\n";
    
    std::cout << "3. 【使用建议】:\n";
    std::cout << "   - 对于简单类型，insert和emplace性能接近\n";
    std::cout << "   - 对于复杂对象，优先使用emplace\n";
    std::cout << "   - 专注于实际有效的优化手段\n";
    
    return 0;
}
