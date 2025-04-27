# mempool.hpp



## unique_pool

定义：`template<typename T> class unique_pool {...};`

这是一个内存池类，它只负责管理某个特定类型的对象的内存.

需要提供类模板参数`T`，表示此内存池管理`T`类型对象的内存.



### 成员变量





### 成员函数

#### 构造函数

```cpp
(1) unique_pool();
(2) unique_pool(ulint);
```

(1) 使用默认构造函数时，



#### init