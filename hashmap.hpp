#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include "utils/__def.hpp"
#include "utils/__errs.hpp" 
#include "utils/xxhash32.hpp"
#include "utils/rbtree.hpp"
#include "utils/bitmap.hpp"
#include "utils/__iterator.hpp"

#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>
#include <initializer_list>
#include <limits>

/**
 * @brief 支持动态桶扩展和红黑树桶的HashMap实现
 * 
 * 主要特性：
 * - 使用32位XXHash算法进行哈希计算，线性映射确保分布均匀
 * - 动态桶数组，负载因子超过0.75时自动扩展
 * - 红黑树作为内部桶结构，保证最坏情况下的O(log n)查找性能
 * - 支持完整的STL兼容接口
 * - 基于位图优化的迭代器实现，提高遍历效率
 * 
 * @tparam Key 键类型
 * @tparam Value 值类型  
 * @tparam Allocator 内存分配器类型，默认为std::allocator
 */
template <typename Key, typename Value, typename Allocator = std::allocator<std::pair<const Key, Value>>>
class HashMap {
public:
    // 类型定义
    using key_type                = Key;                            // 键
    using value_type              = Value;                          // 值
    using pair_type               = std::pair<Key, Value>;          // 内部使用的键值对
    using const_pair_type         = std::pair<const Key, Value>;    // 键值对

    using bucket_type             = utils::rbtree<pair_type>;       // 桶
    using box_type                = std::vector<bucket_type>;       // 箱
    using box_map_type            = utils::bitmap<>;                // 箱的位图

    using size_type               = unsigned long long;             // 大小
    using allocator_type          = Allocator;                      // 分配器

    using hasher_type             = utils::XXHash32;                // 哈希器

private:
    struct box_manager {
      box_type                    box;                // 箱
      box_map_type                box_map;            // 箱的位图
      size_type                   used_bucket_count;  // 非空桶的数量

      box_manager(size_type capacity,
          _rbtree_hpp::search_tree_t<pair_type>::comparer_type comarer,
          _rbtree_hpp::search_tree_t<pair_type>::equaler_type equaler) {
        this->box = box_type(capacity, bucket_type(comparer, equaler));
        this->box_map.init(capacity);
        this->used_bucket_count = 0;
      }
    };

private:
    std::list<box_manager>        box_list;           // 箱链表
    size_type                     box_capacity;       // 每箱容纳多少桶
    size_type                     size;               // 总的键值对数量

    hasher_type                   hasher;             // 哈希器

                                                      // 负载因子阈值
    static constexpr double       LOAD_FACTOR_THRESHOLD = 0.75; 

    
    Allocator allocator;                              // 内存分配器

private:    // 内部函数
    /**
     * @brief 根据预估数据规模计算箱大小
     * 
     * @param estimated_size 预估的键值对数量
     * @return 计算得到的箱大小（2的幂次）
     */
    size_type calculate_initial_box_capacity(size_type estimated_size) const {
        if (estimated_size <= 16) return 16;  // 默认最小大小
        
        // 基于负载因子计算: box_capacity = estimated_size / 0.75
        size_type required_capacity = static_cast<size_type>(estimated_size / LOAD_FACTOR_THRESHOLD) + 1;
        
        // 向上取整到下一个2的幂，以获得更好的哈希分布
        size_type box_capacity = 1;
        while (box_capacity < required_capacity) box_capacity <<= 1;

        return box_capacity;
    }

    /**
     * @brief 使用线性映射获取桶索引
     * 
     * @param key 要计算索引的键
     * @return 对应的桶索引
     */
    size_type get_bucket_index(const key_type& key) const {
        return this->hasher.hash_linear(&key, sizeof(key_type), 0, this->box_capacity - 1);
    }

    /**
     * @brief 根据负载因子检查是否需要扩展
     * 
     * @return true表示需要扩展，false表示不需要
     */
    bool should_expand() const {
        return 
            static_cast<double>(
                this->box_list.back().used_bucket_count
            ) / this->bucket_capacity_
            > LOAD_FACTOR_THRESHOLD;
    }

    /** TODO
     * @brief 扩展箱
     * 
     */
    void expand_box() {}

    /**
     * @brief 在桶中查找元素，考虑比较逻辑
     * 
     * @param bucket 要搜索的桶
     * @param key 要查找的键
     * @return 找到的键值对指针，如果未找到则返回nullptr
     */
    value_type* find_in_bucket(const bucket_type& bucket, const key_type& key) {
      unsigned char kv_buffer[sizeof(pair_type)] {0};
      pair_type* kv = reinterpret_cast<pair_type *>(kv_buffer);
      kv->first = key;

      pair_type *ret = bucket.find(*kv);
      return const_cast<const_pair_type *>(ret);
    }

    /**
     * @brief 更新键值对的值
     * 
     * @param oldpair 旧键值对指针
     * @param value 新的值
     */
    void update_node_value(pair_type *oldpair, const value_type& value) {
        oldpair->second = value;
    }

public:     // 公共函数
    /** TODO
     * @brief HashMap的STL兼容迭代器类
     * 
     * 支持前向和后向遍历，符合STL双向迭代器标准。
     */
    class iterator : utils::_iterator<const_pair_type*, iterator> {
      public:
        using utils::_iterator<const_pair_type*, iterator>::_iterator;

        void goback() {
        }

        void goback(size_t n) {
        }

        void gofront() {
        }

        void gofront(size_t n) {
        }
    };

    /**
     * @brief 带预估大小的构造函数
     * 
     * 根据预估的元素数量计算合适的初始桶大小，以减少后续扩展操作。
     * 
     * @param estimated_size 预估的元素数量，默认为0
     */
    explicit HashMap(size_type estimated_size = 0) 
        : size(0) {
        this->box_capacity = calculate_initial_box_capacity(estimated_size);
        this->box_list.emplace_back(
            this->box_capacity,
            bucket_type(
                [] (const pair_type &a, const pair_type &b) -> bool { return a.first < b.first; },
                [] (const pair_type &a, const pair_type &b) -> bool { return a.first == b.first; })
        );
    }

    /**
     * @brief 范围构造函数
     * 
     * 从迭代器范围构造HashMap。
     * 
     * @tparam InputIt 输入迭代器类型
     * @param first 范围开始迭代器
     * @param last 范围结束迭代器
     * @param estimated_size 预估元素数量
     */
    template<typename InputIt>
    HashMap(InputIt first, InputIt last, size_type estimated_size = 0) : HashMap(estimated_size) {
        for (auto it = first; it != last; ++it) {
            insert(it->first, it->second);
        }
    }

    /**
     * @brief 初始化列表构造函数
     * 
     * 从初始化列表构造HashMap。
     * 
     * @param init 初始化列表
     * @param estimated_size 预估元素数量
     */
    HashMap(std::initializer_list<pair_type> init, size_type estimated_size = 0) : HashMap(estimated_size) {
        for (const auto& pair : init) {
            insert(pair.first, pair.second);
        }
    }

    /** TODO
     * @brief 拷贝构造函数
     * 
     * 创建另一个HashMap的深度拷贝。
     * 
     * @param other 要拷贝的HashMap
     */
    HashMap(const HashMap& other) {}

    /** TODO
     * @brief 移动构造函数
     * 
     * 通过移动语义从另一个HashMap构造，避免不必要的拷贝。
     * 
     * @param other 要移动的HashMap
     */
    HashMap(HashMap&& other) noexcept {}

    /**
     * @brief 拷贝赋值操作符
     * 
     * 使用拷贝和交换惯用法实现异常安全的赋值。
     * 
     * @param other 要拷贝的HashMap
     * @return 当前对象的引用
     */
    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            HashMap tmp(other);
            *this = std::move(tmp);
        }
        return *this;
    }

    /**
     * @brief 移动赋值操作符
     * 
     * 通过移动语义赋值，提高性能。
     * 
     * @param other 要移动的HashMap
     * @return 当前对象的引用
     */
    HashMap& operator=(HashMap&& other) noexcept { }

    /**
     * @brief 析构函数
     * 
     * 使用默认析构函数，因为所有成员都有适当的析构函数。
     */
    ~HashMap() = default;

//#######################################################################################

    /**
     * @brief 插入或更新键值对
     * 
     * 如果键不存在则插入新元素，如果键已存在则更新其值。
     * 在插入前会检查负载因子，必要时进行桶扩展。
     * 
     * @param key 要插入的键
     * @param value 要插入的值
     * @return pair<iterator, bool>，iterator指向插入的元素，bool表示是否为新插入
     */
    std::pair<iterator, bool> insert(const Key& key, const mapped_type& value) {
        // 检查是否需要扩展
        if (should_expand()) {
            expand_buckets();
        }
        
        size_type bucket_idx = get_bucket_index(key);
        bucket_type& bucket = buckets_[bucket_idx];
        
        // 检查键是否已存在于桶中
        auto* existing = find_in_bucket(bucket, key);
        if (existing) {
            // 更新现有值
            update_node_value(existing, value);
            return std::make_pair(iterator::make_iterator_for_node(this, existing), false);
        }
        
        // 插入新元素
        auto* new_node = bucket.push(std::make_pair(key, value));
        bucket_bitmap_.set(bucket_idx, true);
        ++size_;
        
        return std::make_pair(iterator::make_iterator_for_node(this, new_node), true);
    }

    /**
     * @brief 插入或更新键值对（移动语义版本）
     * 
     * 使用移动语义避免不必要的拷贝操作，提高性能。
     * 
     * @param key 要插入的键（右值引用）
     * @param value 要插入的值（右值引用）
     * @return pair<iterator, bool>，iterator指向插入的元素，bool表示是否为新插入
     */
    std::pair<iterator, bool> insert(Key&& key, mapped_type&& value) {
        if (should_expand()) {
            expand_buckets();
        }
        
        size_type bucket_idx = get_bucket_index(key);
        bucket_type& bucket = buckets_[bucket_idx];
        
        auto* existing = find_in_bucket(bucket, key);
        if (existing) {
            update_node_value(existing, std::move(value));
            return std::make_pair(iterator::make_iterator_for_node(this, existing), false);
        }
        
        auto* new_node = bucket.push(std::make_pair(std::move(key), std::move(value)));
        bucket_bitmap_.set(bucket_idx, true);
        ++size_;
        
        return std::make_pair(iterator::make_iterator_for_node(this, new_node), true);
    }

    /**
     * @brief 根据键查找元素（多桶搜索）
     * 
     * 首先在主桶中搜索，如果未找到则在其他可能的桶中搜索。
     * 这是为了处理桶扩展后元素可能分布在不同桶中的情况。
     * 
     * @param key 要查找的键
     * @return 指向找到元素的迭代器，如果未找到则返回end()
     */
    iterator find(const Key& key) {
        // 在主桶中搜索
        size_type bucket_idx = get_bucket_index(key);
        bucket_type& bucket = buckets_[bucket_idx];
        
        auto* node = find_in_bucket(bucket, key);
        if (node) {
            // 创建一个迭代器并找到该键的位置
            iterator it = begin();
            while (it != end()) {
                if (it->first == key) {
                    return it;
                }
                ++it;
            }
        }
        
        // 多桶搜索：检查元素是否可能在其他桶中
        // 由于之前的扩展操作，元素可能分布在不同的桶中。
        // 这需要在该键在之前容量大小时可能映射到的桶范围内搜索。
        
        size_type capacity = bucket_capacity_;
        while (capacity > 16) {  // 搜索之前的容量级别
            capacity /= 2;
            size_type alt_bucket_idx = hasher_.hash_linear(&key, sizeof(Key), 0, capacity - 1);
            
            if (alt_bucket_idx != bucket_idx && alt_bucket_idx < bucket_capacity_) {
                bucket_type& alt_bucket = buckets_[alt_bucket_idx];
                auto* alt_node = find_in_bucket(alt_bucket, key);
                if (alt_node) {
                    // 创建一个迭代器并找到该键的位置
                    iterator it = begin();
                    while (it != end()) {
                        if (it->first == key) {
                            return it;
                        }
                        ++it;
                    }
                }
            }
        }
        
        return end();
    }

    /**
     * @brief 根据键删除元素
     * 
     * 在HashMap中查找指定键的元素并删除。首先在主桶中搜索，
     * 如果未找到则进行多桶搜索以处理扩展后的情况。
     * 
     * @param key 要删除的键
     * @return true如果找到并删除了元素，false如果未找到
     */
    bool erase(const Key& key) {
        // 首先在主桶中搜索
        size_type bucket_idx = get_bucket_index(key);
        bucket_type& bucket = buckets_[bucket_idx];
        
        // 从桶中收集除要删除元素之外的所有元素
        bool found = false;
        std::vector<pair_type> bucket_elements;
        
        bucket.trav_in([&bucket_elements, &key, &found](typename bucket_type::node_type* node, unsigned int level, 
                                          _rbtree_hpp::left_or_right_e pos) {
            (void)level; // 消除未使用参数警告
            (void)pos;   // 消除未使用参数警告
            
            if (node && node->value.first != key) {
                // 保留这个元素
                bucket_elements.push_back(node->value);
            } else if (node) {
                // 找到要删除的元素
                found = true;
            }
        });
        
        if (found) {
            // 创建一个新的空桶
            bucket = bucket_type();
            
            // 重新插入除被删除元素之外的所有元素
            for (const auto& pair : bucket_elements) {
                bucket.push(pair);
            }
            
            // 如果桶变空则更新位图
            if (bucket.size() == 0) {
                bucket_bitmap_.set(bucket_idx, false);
            }
            
            --size_;
            return true;
        }
        
        // 多桶搜索：检查元素是否可能在其他桶中
        size_type capacity = bucket_capacity_;
        while (capacity > 16) {  // 搜索之前的容量级别
            capacity /= 2;
            size_type alt_bucket_idx = hasher_.hash_linear(&key, sizeof(Key), 0, capacity - 1);
            
            if (alt_bucket_idx != bucket_idx && alt_bucket_idx < bucket_capacity_) {
                bucket_type& alt_bucket = buckets_[alt_bucket_idx];
                
                // 从备选桶中收集除要删除元素之外的所有元素
                found = false;
                std::vector<pair_type> alt_bucket_elements;
                
                alt_bucket.trav_in([&alt_bucket_elements, &key, &found](typename bucket_type::node_type* node, unsigned int level, 
                                                      _rbtree_hpp::left_or_right_e pos) {
                    (void)level; // 消除未使用参数警告
                    (void)pos;   // 消除未使用参数警告
                    
                    if (node && node->value.first != key) {
                        // 保留这个元素
                        alt_bucket_elements.push_back(node->value);
                    } else if (node) {
                        // 找到要删除的元素
                        found = true;
                    }
                });
                
                if (found) {
                    // 创建一个新的空桶
                    alt_bucket = bucket_type();
                    
                    // 重新插入除被删除元素之外的所有元素
                    for (const auto& pair : alt_bucket_elements) {
                        alt_bucket.push(pair);
                    }
                    
                    // 如果桶变空则更新位图
                    if (alt_bucket.size() == 0) {
                        bucket_bitmap_.set(alt_bucket_idx, false);
                    }
                    
                    --size_;
                    return true;
                }
            }
        }
        
        return false;
    }

    /**
     * @brief 通过迭代器删除元素
     * 
     * 删除迭代器指向的元素，并返回指向下一个元素的迭代器。
     * 
     * @param it 指向要删除元素的迭代器
     * @return 指向下一个元素的迭代器，如果删除的是最后一个元素则返回end()
     */
    iterator erase(iterator it) {
        if (it == end()) {
            return end();
        }
        
        // 获取要删除的键并找到下一个迭代器
        Key key = it->first;
        iterator next_it = it;
        ++next_it;
        
        // 使用键版本的erase删除元素
        erase(key);
        
        // 无论erase是否成功都返回保存的下一个迭代器
        // 这保持了STL风格的行为
        return next_it;
    }

    /**
     * @brief 删除一个范围内的元素
     * 
     * @param first 范围开始的迭代器
     * @param last 范围结束的迭代器（不包含）
     * @return 指向last的迭代器
     */
    iterator erase(iterator first, iterator last) {
        iterator it = first;
        while (it != last) {
            it = erase(it);
        }
        return last;
    }

    /**
     * @brief 通过键访问元素（如果不存在则插入）
     * 
     * 如果键存在则返回对应值的引用，如果不存在则插入默认值并返回引用。
     * 这是STL map的标准行为。
     * 
     * @param key 要访问的键
     * @return 对应值的引用
     */
    mapped_type& operator[](const Key& key) {
        // 首先尝试查找现有的键
        auto it = find(key);
        if (it != end()) {
            return it->second;
        }
        
        // 键不存在，插入默认值
        auto result = insert(key, mapped_type{});
        return result.first->second;
    }

    /**
     * @brief 获取元素数量
     * 
     * @return 当前HashMap中的元素总数
     */
    size_type size() const {
        return size_;
    }

    /**
     * @brief 检查map是否为空
     * 
     * @return true如果HashMap为空，false否则
     */
    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief 获取当前桶数量
     * 
     * @return 当前桶数组的大小
     */
    size_type bucket_count() const {
        return bucket_capacity_;
    }

    /**
     * @brief 获取当前负载因子
     * 
     * 负载因子是元素数量与桶数量的比值，用于判断是否需要扩展。
     * 
     * @return 当前的负载因子值
     */
    double load_factor() const {
        return static_cast<double>(size_) / bucket_capacity_;
    }

    /**
     * @brief 获取指向开始的迭代器
     * 
     * @return 指向第一个元素的迭代器，如果为空则返回end()
     */
    iterator begin() {
        return iterator(this, false);  // false = 不是end迭代器
    }

    /**
     * @brief 获取指向结束的迭代器
     * 
     * @return 指向最后一个元素之后位置的迭代器
     */
    iterator end() {
        return iterator(this, true);   // true = end迭代器
    }

    /**
     * @brief 清空所有元素
     * 
     * 删除HashMap中的所有元素，但保持桶数组的大小不变。
     * 清空后HashMap的大小为0，但容量保持原值。
     */
    void clear() {
        buckets_.clear();
        buckets_.resize(bucket_capacity_);
        
        // 为每个桶配置仅比较键的比较函数
        for (auto& bucket : buckets_) {
            bucket = bucket_type(
                [](const pair_type& a, const pair_type& b) -> bool { 
                    return a.first < b.first; 
                },
                [](const pair_type& a, const pair_type& b) -> bool { 
                    return a.first == b.first; 
                }
            );
        }
        
        // 通过将所有位设置为false来清空位图，而不是重新初始化
        for (size_type i = 0; i < bucket_capacity_; ++i) {
            bucket_bitmap_.set(i, false);
        }
        
        size_ = 0;
        bucket_count_ = 0;
    }

    /**
     * @brief 通过键访问元素，带边界检查
     * 
     * 如果键不存在则抛出异常，这提供了比operator[]更安全的访问方式。
     * 
     * @param key 要访问的键
     * @return 对应值的引用
     * @throws std::out_of_range 如果键不存在
     */
    mapped_type& at(const Key& key) {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("HashMap::at: key not found");
        }
        return it->second;
    }

    /**
     * @brief 通过键访问元素，带边界检查（const版本）
     * 
     * @param key 要访问的键
     * @return 对应值的const引用
     * @throws std::out_of_range 如果键不存在
     */
    const mapped_type& at(const Key& key) const {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("HashMap::at: key not found");
        }
        return it->second;
    }

    /**
     * @brief 检查键是否存在于map中
     * 
     * @param key 要检查的键
     * @return true如果键存在，false否则
     */
    bool contains(const Key& key) const {
        return find(key) != end();
    }

    /**
     * @brief 重新哈希map使其至少有bucket_count个桶
     * 
     * 如果指定的桶数量大于当前桶数量，则重新分配桶数组并重新分布所有元素。
     * 
     * @param bucket_count 期望的最小桶数量
     */
    void rehash(size_type bucket_count) {
        if (bucket_count <= bucket_capacity_) {
            return; // 无需重新哈希
        }

        // 保存当前元素
        std::vector<pair_type> all_elements;
        all_elements.reserve(size_);

        // 提取所有元素
        for (auto it = begin(); it != end(); ++it) {
            all_elements.push_back(*it);
        }
        
        // 调整为新的桶数量
        bucket_capacity_ = bucket_count;
        buckets_.clear();
        buckets_.resize(bucket_capacity_);
        
        // 配置每个桶
        for (auto& bucket : buckets_) {
            bucket = bucket_type(
                [](const pair_type& a, const pair_type& b) -> bool { 
                    return a.first < b.first; 
                },
                [](const pair_type& a, const pair_type& b) -> bool { 
                    return a.first == b.first; 
                }
            );
        }
        
        bucket_bitmap_.init(bucket_capacity_);
        size_ = 0;
        bucket_count_ = 0;
        
        // 重新分布元素
        for (const auto& element : all_elements) {
            insert(element.first, element.second);
        }
    }

    /**
     * @brief 为至少指定数量的元素预留空间
     * 
     * 根据预期的元素数量和负载因子计算需要的桶数量，必要时进行重新哈希。
     * 
     * @param count 预期的元素数量
     */
    void reserve(size_type count) {
        size_type required_buckets = static_cast<size_type>(count / LOAD_FACTOR_THRESHOLD) + 1;
        if (required_buckets > bucket_capacity_) {
            rehash(required_buckets);
        }
    }

    /**
     * @brief 带提示插入元素（在此实现中忽略迭代器提示）
     * 
     * @param hint 插入位置的提示迭代器（在此实现中未使用）
     * @param value 要插入的值
     * @return 指向插入元素的迭代器
     */
    iterator insert(const_iterator hint, const value_type& value) {
        (void)hint; // 在此实现中未使用
        auto result = insert(value.first, value.second);
        return result.first;
    }

    /**
     * @brief 带移动语义和提示的插入元素
     * 
     * @param hint 插入位置的提示迭代器（在此实现中未使用）
     * @param value 要移动插入的值
     * @return 指向插入元素的迭代器
     */
    iterator insert(const_iterator hint, value_type&& value) {
        (void)hint; // 在此实现中未使用
        auto result = insert(std::move(value.first), std::move(value.second));
        return result.first;
    }

    /**
     * @brief 插入一个范围内的元素
     * 
     * @tparam InputIt 输入迭代器类型
     * @param first 范围开始迭代器
     * @param last 范围结束迭代器
     */
    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            insert(it->first, it->second);
        }
    }

    /**
     * @brief 从初始化列表插入元素
     * 
     * @param ilist 包含要插入元素的初始化列表
     */
    void insert(std::initializer_list<value_type> ilist) {
        for (const auto& pair : ilist) {
            insert(pair.first, pair.second);
        }
    }

    /**
     * @brief 就地构造元素
     * 
     * 使用提供的参数直接在HashMap中构造新元素，避免不必要的拷贝或移动。
     * 
     * @tparam Args 构造参数类型
     * @param args 用于构造元素的参数
     * @return pair<iterator, bool>，iterator指向构造的元素，bool表示是否为新插入
     */
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        pair_type pair(std::forward<Args>(args)...);
        return insert(std::move(pair.first), std::move(pair.second));
    }

    /**
     * @brief 带提示就地构造元素
     * 
     * @tparam Args 构造参数类型
     * @param hint 插入位置的提示迭代器（在此实现中未使用）
     * @param args 用于构造元素的参数
     * @return 指向构造元素的迭代器
     */
    template<typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        (void)hint; // 在此实现中未使用
        auto result = emplace(std::forward<Args>(args)...);
        return result.first;
    }

    /**
     * @brief 获取指向开始的const迭代器
     * 
     * @return 指向第一个元素的const迭代器
     */
    const_iterator begin() const {
        return const_cast<HashMap*>(this)->begin();
    }

    /**
     * @brief 获取指向结束的const迭代器
     * 
     * @return 指向最后一个元素之后位置的const迭代器
     */
    const_iterator end() const {
        return const_cast<HashMap*>(this)->end();
    }

    /**
     * @brief 获取指定键的const迭代器
     * 
     * @param key 要查找的键
     * @return 指向找到元素的const迭代器，如果未找到则返回end()
     */
    const_iterator find(const Key& key) const {
        return const_cast<HashMap*>(this)->find(key);
    }

    /**
     * @brief 调试函数，打印哈希表内容
     * 
     * 输出HashMap的调试信息，包括大小、桶容量、负载因子等统计信息。
     */
    void debug() const {
        std::cout << "HashMap调试信息:\n";
        std::cout << "  大小: " << size_ << "\n";
        std::cout << "  桶容量: " << bucket_capacity_ << "\n";
        std::cout << "  负载因子: " << load_factor() << "\n";
        std::cout << "  非空桶数量: " << bucket_count_ << "\n";
        
        std::cout << "  桶内容:\n";
        for (size_type i = 0; i < bucket_capacity_; ++i) {
            if (bucket_bitmap_.get(i)) {
                std::cout << "    桶 " << i << ": 包含元素 (树大小: " << buckets_[i].size() << ")\n";
            }
        }
    }

    /**
     * @brief 获取分配器
     * 
     * @return 当前使用的分配器对象
     */
    allocator_type get_allocator() const {
        return allocator_;
    }

    /**
     * @brief 获取最大可能的大小
     * 
     * @return 理论上HashMap可以容纳的最大元素数量
     */
    size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }

    /**
     * @brief 获取最大负载因子
     * 
     * @return 当前设置的最大负载因子（固定为0.75）
     */
    double max_load_factor() const {
        return LOAD_FACTOR_THRESHOLD;
    }

    /**
     * @brief 设置最大负载因子（未实现 - 固定为0.75）
     * 
     * @param ml 要设置的最大负载因子（此参数会被忽略）
     */
    void max_load_factor(double ml) {
        (void)ml; // 忽略 - 负载因子固定
    }
};

#endif // HASHMAP_HPP
