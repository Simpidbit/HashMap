#ifndef HASHMAP_UTILS_QUEUE_HPP
#define HASHMAP_UTILS_QUEUE_HPP

#include "__def.hpp"
#include "__iterator.hpp"
#include "__errs.hpp"
#include <iostream>


namespace _utils_constants {

static const size_t STATIC_DEQUE_DEFAULT_CCOUNT = 5;

}

namespace utils {


/**
 * @brief static_deque 是一个最大容量固定的双端队列，它可以在队首和队尾新增、删除元素.
 * @tparam T 元素类型
 * @tparam Allocator 分配器类型
 */
template <typename T, typename Allocator = std::allocator<T> >
class static_deque {
  public:
    friend class iterator;
    class iterator : public utils::_iterator<T*, iterator> {
      protected:
        static_deque *self {nullptr};

      public:
        using utils::_iterator<T*, iterator>::_iterator;
        void setself(static_deque *self) {
          this->self = self;
        }

        void goback() override {
          self->circle_backstep(this->ptr);
        }
        void goback(size_t n) override {
          for (size_t i = 0; i != n; i++)
            this->goback();
        }
        void gofront() override {
          self->circle_frontstep(this->ptr);
        }
        void gofront(size_t n) override {
          for (size_t i = 0; i != n; i++)
            this->gofront();
        }
    };

  protected:
    T *membegin = nullptr;  // 内存块首地址
    T *memback = nullptr;   // 内存块末地址后一位置
    T *head = nullptr;      // 逻辑首元素前一逻辑位置
    T *tail = nullptr;      // 逻辑末元素后一逻辑位置
    size_t ccount = 0;   // capacity
    size_t ecount = 0;   // element
    Allocator allocator;
  

  protected:
    void insert_at(T *ptr, const T &element) { *ptr = element; }

    void insert_at(T *ptr, T &&element) { *ptr = std::move(element); }

    void circle_backstep(T* &ptr) {
      if (ptr == this->memback) ptr = this->membegin;
      else ptr++;
    }

    void circle_frontstep(T* &ptr) {
      if (ptr == this->membegin) ptr = this->memback;
      else ptr--;
    }


  public:
    /**
     * @brief 初始化容器，此函数仅当以默认构造函数构造时才应被调用.
     * @param ccount 最多容纳多少个元素
     * @see static_deque::static_deque()
     */
    void init(size_t ccount) {
      this->ccount = ccount;
      this->membegin = this->allocator.allocate(ccount);
      this->memback = this->membegin + ccount;
      this->head = this->membegin;
      this->tail = this->membegin + 1;
    }

    /**
      * @brief 稍后再手动调用 init 成员函数进行初始化，未初始化之前此容器不可用.
      * @see static_deque::init(size_t)
      */
    static_deque() = default;

    /**
     * @brief 初始化时就指定最大容量.
     * @param ccount 最多容纳多少个元素
     */
    static_deque(size_t ccount) { this->init(ccount); }

    /**
     * @brief 容器目前有多少个元素.
     * @return 容器目前容纳的元素数量.
     */
    inline size_t size() const noexcept { return this->ecount; }

    /**
     * @brief 容器最多能容纳多少个元素.
     * @return 容器最大容量.
     */
    inline size_t capacity() const noexcept { return this->ccount; }

    /**
     * @brief 容器是否为空.
     * @return 容器为空: true; 容器非空: false.
     */
    inline bool empty() const noexcept { return !this->ecount; }


    /**
     * @brief 尝试在队尾通过拷贝的方式插入元素.
     * @param element 要插入的元素（左值）.
     * @return 插入成功: true; 插入失败: false.
     */
    bool push_back(const T &element) {
      T tmp = element;
      return this->push_back(std::move(tmp));
    }

    /**
     * @brief 尝试在队尾通过移动的方式插入元素.
     * @param element 要插入的元素（左值）.
     * @return 插入成功: true; 插入失败: false.
     */
    bool push_back(T &&element) {
      if (this->ecount < this->ccount) {
        this->insert_at(this->tail, std::forward<T>(element));
        this->ecount++;

        this->circle_backstep(this->tail);

        return true;
      } else return false;
    }

    /**
     * @brief 尝试在队首通过拷贝的方式插入元素.
     * @param element 要插入的元素（左值）.
     * @return 插入成功: true; 插入失败: false.
     */
    bool push_front(const T &element) {
      T tmp = element;
      return this->push_front(std::move(tmp));
    }

    /**
     * @brief 尝试在队首通过移动的方式插入元素.
     * @param element 要插入的元素（左值）.
     * @return 插入成功: true; 插入失败: false.
     */
    bool push_front(T &&element) {
      if (this->ecount < this->ccount) {
        this->insert_at(this->head, std::forward<T>(element));
        this->ecount++;

        this->circle_frontstep(this->head);

        return true;
      } else return false;
    }

    /**
     * @brief 如果容器非空，移除队尾元素.
     */
    void pop_back() {
      if (!this->ecount) return;
      this->circle_frontstep(this->tail);
      this->ecount--;
    }

    /**
     * @brief 如果容器非空，移除队首元素.
     */
    void pop_front() {
      if (!this->ecount) return;
      this->circle_backstep(this->head);
      this->ecount--;
    }

    /**
     * @brief 获取队首元素的引用，可通过此引用修改队首元素.
     * @details 如果容器为空，抛出异常 @ref utils::utils_exception , 异常信息为 "static_deque is empty but front() called!"
     */
    T & front() {
      if (!this->ecount)
        throw utils::utils_exception("static_deque is empty but front() called!");

      T *headtmp = this->head;
      this->circle_backstep(headtmp);
      return *headtmp;
    }

    /**
     * @brief 获取队尾元素的引用，可通过此引用修改队尾元素.
     * @details 如果容器为空，抛出异常 @ref utils::utils_exception , 异常信息为 "static_deque is empty but back() called!"
     */
    T & back() {
      if (!this->ecount)
        throw utils::utils_exception("static_deque is empty but back() called!");

      T *tailtmp = this->tail;
      this->circle_frontstep(tailtmp);
      return *tailtmp;
    }

    /**
     * @brief 获取队首元素的迭代器.
     */
    iterator begin() {
      iterator iter;
      iter.setself(this);
      T *headtmp = this->head;
      this->circle_backstep(headtmp);
      iter.point_to(headtmp);
      return iter;
    }

    /**
     * @brief 获取队尾元素下一位置的迭代器.
     */
    iterator end() {
      iterator iter;
      iter.setself(this);
      T *tailtmp = this->tail;
      iter.point_to(tailtmp);
      return iter;
    }
};


} // namespace utils

#endif
