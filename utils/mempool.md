# mempool.hpp



## unique_pool

定义：`template<typename T> class unique_pool {...};`

这是一个内存池类，它只负责管理某个特定类型的对象的内存.

需要提供类模板参数`T`，表示此内存池管理`T`类型对象的内存.



### 成员变量

- `blk_piece_count`：这个内存池每次向系统申请的内存需要能存储`blk_piece_count`个`T`类型的对象. 默认为 `4096`.



### 成员函数

#### 构造函数

```cpp
(1) unique_pool();
(2) unique_pool(ulint n);
```

(1) 使用默认构造函数时，`blk_piece_count` 成员变量是 `4096`.

(2) 构造时将 `blk_piece_count` 成员变量设为`n`.



#### init

```cpp
void init();
```

必须调用`init()`成员函数后内存池才可用，在`init()`函数调用之前从内存池申请内存，会抛出