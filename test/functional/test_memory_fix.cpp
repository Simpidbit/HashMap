#include "hashmap.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 内存修复验证测试 ===" << std::endl;
    
    HashMap<int, std::string> map;
    
    // 插入大量数据以确保红黑树节点创建正确
    std::cout << "插入大量数据..." << std::endl;
    for (int i = 0; i < 1000; ++i) {
        map[i] = "值_" + std::to_string(i);
    }
    
    std::cout << "当前大小: " << map.size() << std::endl;
    
    // 验证数据正确性
    std::cout << "验证数据正确性..." << std::endl;
    bool all_correct = true;
    for (int i = 0; i < 1000; ++i) {
        if (map[i] != "值_" + std::to_string(i)) {
            std::cout << "错误: 键 " << i << " 的值不正确" << std::endl;
            all_correct = false;
        }
    }
    
    if (all_correct) {
        std::cout << "所有数据验证通过!" << std::endl;
    }
    
    // 测试删除操作
    std::cout << "测试删除操作..." << std::endl;
    for (int i = 0; i < 500; ++i) {
        map.erase(i);
    }
    
    std::cout << "删除后大小: " << map.size() << std::endl;
    
    // 验证剩余数据
    std::cout << "验证剩余数据..." << std::endl;
    for (int i = 500; i < 1000; ++i) {
        if (map.find(i) == map.end()) {
            std::cout << "错误: 键 " << i << " 应该存在但未找到" << std::endl;
            all_correct = false;
        }
    }
    
    for (int i = 0; i < 500; ++i) {
        if (map.find(i) != map.end()) {
            std::cout << "错误: 键 " << i << " 应该被删除但仍存在" << std::endl;
            all_correct = false;
        }
    }
    
    if (all_correct) {
        std::cout << "删除操作验证通过!" << std::endl;
    }
    
    std::cout << "=== 内存修复验证完成 ===" << std::endl;
    return 0;
}
