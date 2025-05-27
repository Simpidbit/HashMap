# HashMap - C++ 红黑树哈希表实现

[![Language](https://img.shields.io/badge/Language-C++17-orange.svg)]()
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)]()

一个高性能的C++哈希表实现，使用红黑树解决哈希冲突，支持STL标准接口。

## ✨ 最新更新

- ✅ **修复所有编译错误**: 解决了 `erase()` 方法中的编译问题
- ✅ **性能优化**: `find_in_bucket` 方法现在使用红黑树的 `find()` 方法，提升查找性能
- ✅ **完整测试覆盖**: 11个测试全部通过，包括基础功能、STL兼容性和哈希算法测试
- ✅ **跨平台支持**: 在Windows MinGW环境下完全工作

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

# 配置和编译（Windows MinGW）
cmake -B build -G "MinGW Makefiles"
cmake --build build

# 或者在Unix系统上
cmake -B build
cmake --build build

# 运行测试
cd build
ctest --output-on-failure
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
│   ├── mempool.hpp         # 内存池管理
│   ├── vector.hpp          # 向量容器
│   ├── list.hpp            # 链表容器
│   └── bintree/            # 二叉树基础类
│       └── tree.hpp
├── test/                   # 测试文件目录
│   ├── basic/              # 基础功能测试
│   │   ├── simple_test.cpp
│   │   └── test_constructors.cpp
│   ├── functional/         # 功能测试
│   │   ├── test_comprehensive_final.cpp
│   │   ├── test_erase.cpp
│   │   ├── test_expansion.cpp
│   │   ├── test_hashmap.cpp
│   │   ├── test_iterator_issues.cpp
│   │   └── test_memory_fix.cpp
│   ├── stl_compliance/     # STL兼容性测试
│   │   ├── test_basic_stl.cpp
│   │   └── test_stl_compliance.cpp
│   └── hash/               # 哈希算法测试
│       └── test_xxhash.cpp
├── bin/                    # 编译生成的可执行文件
├── build/                  # CMake构建目录
├── CMakeLists.txt          # CMake构建配置
└── README.md               # 项目说明文档
```

## 🧪 测试

项目包含全面的测试套件，按功能模块组织：

```bash
# 运行所有测试
cd build
ctest --output-on-failure

# 运行特定测试（从项目根目录）
./bin/simple_test             # 基础功能测试
./bin/test_constructors       # 构造函数测试
./bin/test_comprehensive_final # 综合功能测试
./bin/test_erase             # 删除操作测试
./bin/test_expansion         # 扩容测试
./bin/test_hashmap           # 哈希表功能测试
./bin/test_iterator_issues   # 迭代器测试
./bin/test_memory_fix        # 内存管理测试
./bin/test_xxhash            # 哈希算法测试
./bin/test_basic_stl         # STL基础兼容性测试
./bin/test_stl_compliance    # STL完整兼容性测试

# 重新构建和测试
cmake --build build && cd build && ctest
```

### 测试类别说明

- **basic/**: 基础功能测试，包括简单操作和构造函数
- **functional/**: 功能测试，包括删除、扩容、内存管理等
- **stl_compliance/**: STL标准接口兼容性测试
- **hash/**: 哈希算法性能和正确性测试

## 📊 性能特点

| 操作 | 平均时间复杂度 | 最坏时间复杂度 |
|------|---------------|---------------|
| 插入 | O(1) | O(log n) |
| 查找 | O(1) | O(log n) |
| 删除 | O(1) | O(log n) |
| 遍历 | O(n) | O(n) |

## 🔧 核心实现特点

### 优化的查找性能
- **红黑树优化**: `find_in_bucket` 方法使用红黑树的 `find()` 方法而非线性遍历
- **时间复杂度**: 桶内查找从 O(n) 优化为 O(log n)
- **哈希冲突处理**: 使用自平衡红黑树存储冲突的键值对

### 动态扩容机制
当负载因子超过阈值时，自动扩容并重新哈希所有元素，保持高性能。

### xxHash32哈希算法
使用xxHash32算法提供快速、均匀的哈希分布，减少冲突概率。

### 内存安全
- 通过全面的测试验证内存安全性
- 支持异常安全的操作
- 正确的资源管理和清理

## 📝 API文档

### 主要接口

- `insert(key, value)` - 插入键值对
- `find(key)` - 查找指定键（优化的O(log n)桶内查找）
- `erase(key)` - 删除指定键
- `operator[](key)` - 访问或插入元素
- `size()` - 获取元素数量
- `empty()` - 检查是否为空
- `clear()` - 清空所有元素

### 迭代器支持

- `begin()` / `end()` - 正向迭代器
- `cbegin()` / `cend()` - 常量迭代器
- 支持范围for循环
- 完全符合STL迭代器规范

### 构造函数

- 默认构造函数
- 拷贝构造函数
- 移动构造函数
- 初始化列表构造函数
- 赋值操作符（拷贝和移动）

## 🤝 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

## 🙏 致谢

- [xxHash](https://github.com/Cyan4973/xxHash) - 快速哈希算法
- 感谢所有贡献者和测试人员

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- 创建GitHub Issue

---

⭐ 如果这个项目对您有帮助，请给它一颗星！
