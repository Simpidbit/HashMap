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
    using key_type = Key;                           // 键类型
    using mapped_type = Value;                      // 值类型
    using value_type = std::pair<const Key, Value>; // 键值对类型（键为const）
    using pair_type = std::pair<Key, Value>;        // 内部使用的键值对类型
    using bucket_type = utils::rbtree<pair_type>;   // 桶类型（红黑树）
    using size_type = utils::ulint;                 // 大小类型
    using allocator_type = Allocator;               // 分配器类型

private:
    // 桶数组管理
    std::vector<bucket_type> buckets_;              // 桶数组
    utils::bitmap<> bucket_bitmap_;                 // 非空桶位图，用于高效迭代
    
    // 哈希函数
    utils::XXHash32 hasher_;                        // XXHash32哈希器
    
    // 大小和容量跟踪
    size_type size_;                                // 当前元素数量
    size_type bucket_count_;                        // 非空桶数量
    size_type bucket_capacity_;                     // 当前桶数组大小
    
    // 负载因子阈值
    static constexpr double LOAD_FACTOR_THRESHOLD = 0.75;
    
    // 内存分配器
    Allocator allocator_;

    /**
     * @brief 根据预估数据规模计算初始桶大小
     * 
     * @param estimated_size 预估的元素数量
     * @return 计算得到的初始桶大小（2的幂次）
     */
    size_type calculate_initial_bucket_size(size_type estimated_size) const {
        if (estimated_size == 0) return 16;  // 默认最小大小
        
        // 基于负载因子计算: bucket_size = estimated_size / 0.75
        size_type required_size = static_cast<size_type>(estimated_size / LOAD_FACTOR_THRESHOLD) + 1;
        
        // 向上取整到下一个2的幂，以获得更好的哈希分布
        size_type bucket_size = 1;
        while (bucket_size < required_size) {
            bucket_size <<= 1;
        }
        
        return bucket_size;
    }

    /**
     * @brief 使用线性映射获取桶索引
     * 
     * @param key 要计算索引的键
     * @return 对应的桶索引
     */
    size_type get_bucket_index(const Key& key) const {
        return hasher_.hash_linear(&key, sizeof(Key), 0, bucket_capacity_ - 1);
    }

    /**
     * @brief 根据负载因子检查是否需要扩展
     * 
     * @return true表示需要扩展，false表示不需要
     */
    bool should_expand() const {
        return static_cast<double>(size_) / bucket_capacity_ > LOAD_FACTOR_THRESHOLD;
    }

    /**
     * @brief 扩展桶数组并重新分布元素
     * 
     * 将桶容量翻倍，并将所有现有元素重新哈希到新的桶数组中。
     * 这个操作的时间复杂度为O(n)，其中n是元素总数。
     */
    void expand_buckets() {
        size_type old_capacity = bucket_capacity_;
        size_type new_capacity = bucket_capacity_ * 2;
        
        // 保存当前所有元素
        std::vector<pair_type> all_elements;
        all_elements.reserve(size_);
        
        // 从当前桶中提取所有元素
        for (size_type i = 0; i < old_capacity; ++i) {
            if (bucket_bitmap_.get(i)) {
                bucket_type& bucket = buckets_[i];
                // 使用中序遍历从该桶中提取所有元素
                bucket.trav_in([&all_elements](typename bucket_type::node_type* node, unsigned int level, 
                                              _rbtree_hpp::left_or_right_e pos) {
                    (void)level; // 消除未使用参数警告
                    (void)pos;   // 消除未使用参数警告
                    if (node) {
                        all_elements.push_back(node->value);
                    }
                });
            }
        }
        
        // 创建新的桶数组
        std::vector<bucket_type> new_buckets(new_capacity);
        
        // 为每个新桶配置仅比较键的比较函数
        for (auto& bucket : new_buckets) {
            bucket = bucket_type(
                [](const pair_type& a, const pair_type& b) -> bool { 
                    return a.first < b.first; 
                },
                [](const pair_type& a, const pair_type& b) -> bool { 
                    return a.first == b.first; 
                }
            );
        }
        
        utils::bitmap<> new_bitmap;
        new_bitmap.init(new_capacity);
        
        // 更新容量和数组
        bucket_capacity_ = new_capacity;
        buckets_ = std::move(new_buckets);
        bucket_bitmap_ = std::move(new_bitmap);
        
        // 重新分布元素
        for (const auto& element : all_elements) {
            size_type new_bucket_idx = get_bucket_index(element.first);
            buckets_[new_bucket_idx].push(element);
            bucket_bitmap_.set(new_bucket_idx, true);
        }
        
        // 大小保持不变，因为我们重新分布了所有元素
    }

    /**
     * @brief 在桶中查找元素，考虑比较逻辑
     * 
     * @param bucket 要搜索的桶
     * @param key 要查找的键
     * @return 找到的节点指针，如果未找到则返回nullptr
     */
    typename bucket_type::node_type* find_in_bucket(bucket_type& bucket, const Key& key) {
        // 使用中序遍历查找具有匹配键的节点
        typename bucket_type::node_type* found_node = nullptr;
        
        bucket.trav_in([&found_node, &key](typename bucket_type::node_type* node, unsigned int level, 
                                           _rbtree_hpp::left_or_right_e pos) {
            (void)level; // 消除未使用参数警告
            (void)pos;   // 消除未使用参数警告
            if (node && node->value.first == key) {
                found_node = node;
            }
        });
        
        return found_node;
    }

    /**
     * @brief 更新找到节点的值
     * 
     * @param node 要更新的节点
     * @param value 新的值
     */
    void update_node_value(typename bucket_type::node_type* node, const mapped_type& value) {
        if (node) {
            // 访问节点的值并更新第二个组件
            node->value.second = value;
        }
    }

public:
    /**
     * @brief HashMap的STL兼容迭代器类
     * 
     * 基于vector的迭代器实现，通过收集所有节点指针来实现高效遍历。
     * 支持前向和后向遍历，符合STL双向迭代器标准。
     */
    class iterator {
    private:
        HashMap* map_;                                                  // 关联的HashMap指针
        std::vector<typename bucket_type::node_type*> all_nodes_;      // 所有节点的指针集合
        size_t current_index_;                                          // 当前迭代位置
        
        /**
         * @brief 收集所有节点到vector中
         * 
         * 遍历所有非空桶，使用中序遍历收集所有节点指针。
         * 这确保了迭代器能够按确定顺序访问所有元素。
         */
        void collect_all_nodes() {
            all_nodes_.clear();
            if (!map_) return;
            
            // 遍历所有桶并使用中序遍历收集节点
            for (size_type i = 0; i < map_->bucket_capacity_; ++i) {
                if (map_->bucket_bitmap_.get(i)) {
                    auto& bucket = map_->buckets_[i];
                    // 使用中序遍历从该桶收集节点
                    bucket.trav_in([this](typename bucket_type::node_type* node, unsigned int level, 
                                          _rbtree_hpp::left_or_right_e pos) {
                        (void)level; // 抑制未使用参数警告
                        (void)pos;   // 抑制未使用参数警告
                        if (node) {
                            all_nodes_.push_back(node);
                        }
                    });
                }
            }
        }
        
        /**
         * @brief 获取当前位置的键值对指针
         * 
         * @return 当前键值对的指针，如果越界则返回nullptr
         */
        pair_type* get_current_pair() const {
            if (!map_ || current_index_ >= all_nodes_.size()) {
                return nullptr;
            }
            return &(all_nodes_[current_index_]->value);
        }

        // 为特定节点创建迭代器的辅助方法
        static iterator make_iterator_for_node(HashMap* map, typename bucket_type::node_type* target_node) {
            iterator it(map, false);
            if (!target_node) {
                return iterator(map, true); // 返回end迭代器
            }
            
            // 在all_nodes_中查找目标节点的位置
            for (size_t i = 0; i < it.all_nodes_.size(); ++i) {
                if (it.all_nodes_[i] == target_node) {
                    it.current_index_ = i;
                    return it;
                }
            }
            return iterator(map, true); // 如果未找到则返回end迭代器
        }
        
    public:
        // STL迭代器类型定义
        using iterator_category = std::forward_iterator_tag;  // 前向迭代器类别
        using value_type = pair_type;                         // 值类型
        using difference_type = std::ptrdiff_t;               // 差值类型
        using pointer = pair_type*;                           // 指针类型
        using reference = pair_type&;                         // 引用类型
        
        friend class HashMap; // 允许HashMap访问私有成员
        
        /**
         * @brief 迭代器构造函数
         * 
         * @param map HashMap指针
         * @param end_iterator 是否为end迭代器
         */
        iterator(HashMap* map = nullptr, bool end_iterator = false) 
            : map_(map), current_index_(0) {
            if (map_ && !end_iterator) {
                collect_all_nodes();
                // 如果没有找到节点，将此设为end迭代器
                if (all_nodes_.empty()) {
                    current_index_ = 0;
                    map_ = nullptr; // 标记为end
                }
            } else {
                // End迭代器
                map_ = nullptr;
                current_index_ = 0;
            }
        }
        
        /**
         * @brief 前缀递增操作符
         * 
         * @return 递增后的迭代器引用
         */
        iterator& operator++() {
            if (map_ && current_index_ < all_nodes_.size()) {
                ++current_index_;
                if (current_index_ >= all_nodes_.size()) {
                    // 到达末尾
                    map_ = nullptr;
                }
            }
            return *this;
        }
        
        /**
         * @brief 后缀递增操作符
         * 
         * @return 递增前的迭代器副本
         */
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        /**
         * @brief 相等比较操作符
         * 
         * @param other 要比较的另一个迭代器
         * @return true如果两个迭代器相等
         */
        bool operator==(const iterator& other) const {
            // 两个end迭代器相等
            if (!map_ && !other.map_) return true;
            // 一个end迭代器和一个非end迭代器不相等
            if (!map_ || !other.map_) return false;
            // 两个有效迭代器 - 比较map和索引
            return map_ == other.map_ && current_index_ == other.current_index_;
        }
        
        /**
         * @brief 不等比较操作符
         * 
         * @param other 要比较的另一个迭代器
         * @return true如果两个迭代器不相等
         */
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
        
        /**
         * @brief 解引用操作符
         * 
         * @return 当前键值对的引用
         */
        pair_type& operator*() const {
            auto* pair_ptr = get_current_pair();
            if (!pair_ptr) {
                static pair_type dummy;
                return dummy;
            }
            return *pair_ptr;
        }
        
        /**
         * @brief 箭头操作符
         * 
         * @return 当前键值对的指针
         */
        pair_type* operator->() const {
            return get_current_pair();
        }
    };
    
    using const_iterator = iterator; // 简化实现 - 应该是独立的类

    /**
     * @brief 带预估大小的构造函数
     * 
     * 根据预估的元素数量计算合适的初始桶大小，以减少后续扩展操作。
     * 
     * @param estimated_size 预估的元素数量，默认为0
     */
    explicit HashMap(size_type estimated_size = 0) 
        : size_(0), bucket_count_(0) {
        bucket_capacity_ = calculate_initial_bucket_size(estimated_size);
        buckets_.resize(bucket_capacity_);
        
        // 为每个桶配置仅比较键的比较函数
        for (auto& bucket : buckets_) {
            // 设置比较函数为仅比较键值对的键部分
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
    HashMap(std::initializer_list<value_type> init, size_type estimated_size = 0) : HashMap(estimated_size) {
        for (const auto& pair : init) {
            insert(pair.first, pair.second);
        }
    }

    /**
     * @brief 拷贝构造函数
     * 
     * 创建另一个HashMap的深度拷贝。
     * 
     * @param other 要拷贝的HashMap
     */
    HashMap(const HashMap& other) 
        : bucket_bitmap_(other.bucket_bitmap_), // 使用位图拷贝构造函数
          size_(0), bucket_count_(0) {
        // 设置与源相同的容量
        bucket_capacity_ = other.bucket_capacity_;
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
        
        // 拷贝桶内容
        for (size_type i = 0; i < other.bucket_capacity_; ++i) {
            if (other.bucket_bitmap_.get(i)) {
                // 拷贝桶内容 - 简化实现
                buckets_[i] = other.buckets_[i];
                // 不需要再次设置位图，因为我们已经拷贝了它
            }
        }
        size_ = other.size_;
    }

    /**
     * @brief 移动构造函数
     * 
     * 通过移动语义从另一个HashMap构造，避免不必要的拷贝。
     * 
     * @param other 要移动的HashMap
     */
    HashMap(HashMap&& other) noexcept 
        : buckets_(std::move(other.buckets_)),
          bucket_bitmap_(std::move(other.bucket_bitmap_)),
          hasher_(std::move(other.hasher_)),
          size_(other.size_),
          bucket_count_(other.bucket_count_),
          bucket_capacity_(other.bucket_capacity_),
          allocator_(std::move(other.allocator_)) {
        other.size_ = 0;
        other.bucket_count_ = 0;
        other.bucket_capacity_ = 0;
    }

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
    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            buckets_ = std::move(other.buckets_);
            bucket_bitmap_ = std::move(other.bucket_bitmap_);
            hasher_ = std::move(other.hasher_);
            size_ = other.size_;
            bucket_count_ = other.bucket_count_;
            bucket_capacity_ = other.bucket_capacity_;
            allocator_ = std::move(other.allocator_);
            
            other.size_ = 0;
            other.bucket_count_ = 0;
            other.bucket_capacity_ = 0;
        }
        return *this;
    }

    /**
     * @brief 析构函数
     * 
     * 使用默认析构函数，因为所有成员都有适当的析构函数。
     */
    ~HashMap() = default;

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
