# HashMap - C++ 红黑树哈希表实现

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language](https://img.shields.io/badge/Language-C++17-orange.svg)]()
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)]()

一个高性能的C++哈希表实现，使用红黑树解决哈希冲突，支持STL标准接口。

## 📋 项目特性

- **红黑树解决冲突**: 使用自平衡红黑树替代传统链表，保证O(log n)最坏情况查找性能
- **STL兼容接口**: 完全兼容STL unordered_map接口规范
- **高性能哈希**: 集成xxHash32算法，提供快速均匀的哈希分布
- **内存安全**: 通过AddressSanitizer验证，无内存泄漏
- **完整测试**: 包含功能测试、性能测试和边界条件测试

## 🚀 快速开始

### 编译要求

- C++17或更高版本编译器
- CMake 3.10+
- 支持平台：Linux, macOS, Windows

### 编译项目

```bash
# 克隆项目（包含子模块）
git clone --recursive https://github.com/yourusername/HashMap.git
cd HashMap

# 创建构建目录
mkdir build && cd build

# 配置和编译
cmake ..
make

# 运行测试
make test
```

### 基本使用示例

```cpp
#include "hashmap.hpp"
#include <iostream>

int main() {
    HashMap<std::string, int> map;
    
    // 插入键值对
    map["apple"] = 5;
    map["banana"] = 3;
    map["orange"] = 8;
    
    // 查找元素
    auto it = map.find("apple");
    if (it != map.end()) {
        std::cout << "apple: " << it->second << std::endl;
    }
    
    // 遍历所有元素
    for (const auto& pair : map) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    
    return 0;
}
```

## 📁 项目结构

```
HashMap/
├── hashmap.hpp              # 主要的HashMap实现
├── utils/                   # 工具库
│   ├── rbtree.hpp          # 红黑树实现
│   ├── xxhash32.hpp        # xxHash32哈希算法
│   └── bintree/            # 二叉树基础类（子模块）
│       └── tree.hpp
├── test/                   # 测试文件目录
│   ├── basic_test.cpp      # 基础功能测试
│   ├── performance_test.cpp # 性能测试
│   ├── stl_test.cpp        # STL兼容性测试
│   └── memory_test.cpp     # 内存安全测试
├── debug/                  # 调试工具和示例
│   ├── debug_basic.cpp     # 基础调试工具
│   ├── debug_performance.cpp # 性能调试工具
│   └── debug_memory.cpp    # 内存调试工具
├── CMakeLists.txt          # CMake构建配置
└── README.md               # 项目说明文档
```

## 🧪 测试

项目包含全面的测试套件：

```bash
# 运行所有测试
make test

# 运行特定测试
./bin/test_basic       # 基础功能测试
./bin/test_performance # 性能测试
./bin/test_stl        # STL兼容性测试
./bin/test_memory     # 内存安全测试

# 内存检查（需要AddressSanitizer）
make test_memory_safe

# 性能分析（需要Valgrind）
make valgrind
```

## 📊 性能特点

| 操作 | 平均时间复杂度 | 最坏时间复杂度 |
|------|---------------|---------------|
| 插入 | O(1) | O(log n) |
| 查找 | O(1) | O(log n) |
| 删除 | O(1) | O(log n) |
| 遍历 | O(n) | O(n) |

## 🔧 配置选项

通过修改`hashmap.hpp`中的预编译定义来调整HashMap行为：

```cpp
// 启用调试输出
#define HASHMAP_DEBUG 1

// 调整初始容量
#define DEFAULT_CAPACITY 16

// 调整负载因子阈值
#define LOAD_FACTOR_THRESHOLD 0.75
```

## 🐛 调试工具

项目提供了多种调试工具帮助开发：

```bash
# 基础功能调试
./bin/debug_basic

# 性能分析调试
./bin/debug_performance

# 内存使用调试
./bin/debug_memory

# 代码格式化
make format
```

## 🔄 核心算法

### 红黑树冲突解决
当哈希冲突发生时，使用红黑树存储冲突的键值对，保证最坏情况下O(log n)的查找性能。

### 动态扩容
当负载因子超过阈值时，自动扩容并重新哈希所有元素，保持高性能。

### xxHash32哈希算法
使用xxHash32算法提供快速、均匀的哈希分布，减少冲突概率。

## 📝 API文档

### 主要接口

- `insert(key, value)` - 插入键值对
- `find(key)` - 查找指定键
- `erase(key)` - 删除指定键
- `operator[](key)` - 访问或插入元素
- `size()` - 获取元素数量
- `empty()` - 检查是否为空
- `clear()` - 清空所有元素

### 迭代器支持

- `begin()` / `end()` - 正向迭代器
- `cbegin()` / `cend()` - 常量迭代器
- 支持范围for循环

## 🤝 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

## 📄 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- [xxHash](https://github.com/Cyan4973/xxHash) - 快速哈希算法
- 感谢所有贡献者和测试人员

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- 创建GitHub Issue
- 发送邮件到：[your-email@example.com]

---

⭐ 如果这个项目对您有帮助，请给它一颗星！
