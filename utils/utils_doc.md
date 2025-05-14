目前已完成的功能：

| 名称              | 功能                                                         | 头文件              |
| ----------------- | ------------------------------------------------------------ | ------------------- |
| bitmap            | 位图，是 mempool 的依赖类.                                   | `utils/bitmap.hpp`  |
| mempool_allocator | 内存池分配器，是一个内存池，同时也符合C++标准约定的分配器（Allocator）接口. | `utils/mempool.hpp` |
| static_deque      | 固定容量上限的双端队列.                                      | `utils/queue.hpp`   |
| vector            | 动态数组.                                                    | `utils/vector.hpp`  |
|                   |                                                              |                     |



# 接口文档

## static_deque

#### class 声明头

```cpp
template <typename T, typename Allocator = std::allocator<T> >
class static_deque;
```

| 名字        | 解释                  |
| ----------- | --------------------- |
| `T`         | 容器要容纳的元素类型. |
| `Allocator` | 容器的内存分配器类型. |



### 成员函数

#### 构造函数

| 函数原型                                   | 解释                                  |
| ------------------------------------------ | ------------------------------------- |
| `static_deque();`                          | 默认会初始化一个容量为 5 的容器.      |
| `static_deque(size_t ccount);`             | 指定初始化一个容量为 `ccount` 的容器. |



#### 查询容器状态

| 函数名 | 原型                                   | 解释                                       |
| ----- | ------------------------------------------ | ------------------------------------------ |
| size | `inline size_t size() const noexcept;`     | 返回容器当前容纳的元素个数.                |
| capacity | `inline size_t capacity() const noexcept;` | 返回容器最多能容纳多少个元素.              |
| empty | `inline bool empty() const noexcept;`      | 返回容器是否非空，若容器为空则返回 `true`. |



#### 元素操作

| 函数名 | 函数原型                            | 解释                                                         |
| ---- | ----------------------------------- | ------------------------------------------------------------ |
| push_back | (1) `bool push_back(const T &element);`<br>(2) `bool push_back(T &&element);` | (1) 如果容器不满，则将元素以拷贝的方式追加到容器末尾并返回 `true`，否则什么也不做，直接返回 `false`.<br>(2) 如果容器不满，则将元素以移动的方式追加到容器末尾并返回 `true`，否则什么也不做，直接返回 `false`. |
| push_front| (1) `bool push_back(const T &element);`<br>(2) `bool push_back(T &&element);` | (1) 如果容器不满，则将元素以拷贝的方式追加到容器末尾并返回 `true`，否则什么也不做，直接返回 `false`.<br/>(2) 如果容器不满，则将元素以移动的方式追加到容器末尾并返回 `true`，否则什么也不做，直接返回 `false`. |
| |  |  |

