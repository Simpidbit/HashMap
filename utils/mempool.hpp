#ifndef HASHMAP_UTILS_MEMPOOL_HPP
#define HASHMAP_UTILS_MEMPOOL_HPP


#include "__errs.hpp"
#include "__def.hpp"
#include "bitmap.hpp"
#include "queue.hpp"

#include <thread>
#include <atomic>


namespace utils {

template <typename T>
class _abs_allocator {
  public:
    using value_type = T;

  public:
    virtual T* allocate(size_t n) = 0;
    virtual void deallocate(T *p, size_t n) = 0;
};

/*
mempool_allocator:
  初始时是一大块内存区域.
  定义
    内存状态图      memory_state_map
    待分配指针队列  allocating_queue
    待释放指针队列  deallocating_queue

  分配
    取待分配指针队列前端的指针分配给外部.
    发送信号给守护线程，信号包括分配操作信号和被分配指针.

    详细过程
      检查

  释放
    将待释放指针放入待释放指针队列队尾.
    发送信号给守护线程，信号包括释放操作信号.


  守护线程
    会根据若干信号工作.
    信号
      tran_signal_for_daemon (bool)
        守护线程的超越信号，用于指示守护线程发出的新信号的出现.
        静息状态为 false.
        由守护线程触发为动作状态，主线程收到后重置为静息状态.
    &
      tran_signal_for_main (bool)
        主线程的超越信号，用于指示主线程发出的新信号的出现.
        静息状态为 false.
        由主线程触发为动作状态，守护线程收到后重置为静息状态.
      -----------------
      type_signal_for_daemon (unsigned char)
        信号类型信号，用于指示守护线程发出的信号的类型.
    &
      type_signal_for_main (unsigned char)
        信号类型信号，用于指示主线程发出的信号的类型.

        信号类型表
          0x01 - 

  发送信号的流程
    (1) 脉冲信号 A --S(X)--> B
      线程A
        S <- X;
        type_signal_for_A <- typeid(S);
        tran_signal_for_A <- true;

      线程B
        if tran_signal_for_A:
          signal_type <- type_signal_for_A;
          rcp_S <- S;

          tran_signal_for_A <- false;
          type_signal_for_A <- 0;
          S <- 0;
    (2) 状态信号 A --X--> S
*/

template <typename T>
class mempool_allocator : public _abs_allocator<T> {
  protected:
    bitmap<>          memory_state_map;
    static_deque<T*>  allocating_queue;
    static_deque<T*>  deallocating_queue;

    std::atomic<bool> tran_signal_for_daemon {false};
    std::atomic<bool> tran_signal_for_main {false};

    std::atomic<unsigned char> type_signal_for_daemon {0};
    std::atomic<unsigned char> type_signal_for_main {0};

    std::atomic<bool> 

    std::atomic<unsigned char> operation_signal;
    std::atomic<T *> ptr_signal;

  protected:
    // 守护线程
    static void daemon() {
      
    }

  public:
    mempool_allocator() {
      std::thread(mempool_allocator::daemon).detach();
    }

    T* allocate(size_t n) override {
      if (this->allocating_queue.empty()) {
        // 等待守护线程寻找空闲内存
        for (;;) if (this->tran_signal_for_daemon) break;
        switch (this->type_signal_for_daemon)
      } else {
        return this->allocating_queue.top();
        // 给守护线程发信号
      }
    }

    void deallocate(T *p, size_t n) {
      // 给守护线程发信号
    }
};




/*


template <typename T>
class unique_pool {
  protected:
    T *membegin = nullptr;
    T *memend = nullptr;
    T *availptr = nullptr;
    T *tailptr = nullptr;
    T *last_availptr = nullptr;

    size_t blk_piece_count = 1024 * 4;   // 默认: 4096
    size_t used_count = 0;
    size_t avail_count = 0;
    std::thread search_thread;

    bitmap<> blkmap {0b1111'1111};

  public:
    unique_pool(bool is_init = true) {
      if (is_init)
        this->init();
    }

    unique_pool(size_t n, bool is_init = true) : blk_piece_count(n) {
      if (is_init)
        this->init();
    }

    ~unique_pool() {
      delete[] static_cast<T *>(this->membegin);
    }

    void init() {
      this->blkmap.init(this->blk_piece_count);
      try {
        this->membegin = new T[this->blk_piece_count];
        this->memend = this->membegin + this->blk_piece_count;
        this->tailptr = this->membegin;
        this->availptr = this->membegin;
        this->last_availptr = this->availptr;
      } catch (std::bad_alloc &e) {
        throw utils_exception("unique_pool::init(): Too huge memory wanted!");
      }

      this->avail_count = this->blk_piece_count;

      auto search_loop = [this]() -> void {
        for (;;) {
          if (!this->availptr) {
            for (size_t byte_offset = this->last_availptr - this->membegin;
                byte_offset < this->blk_piece_count; byte_offset++) {
              if (this->blkmap.bits[byte_offset]) {
                size_t bit_offset = 0;
                for (size_t bit_offset = 0; bit_offset < 8; bit_offset++) {
                  if (
                      (static_cast<unsigned char>(0b0000'0001) << bit_offset)
                      & this->blkmap.bits[byte_offset]) {
                    this->availptr = this->membegin + (byte_offset * sizeof(unsigned char)) + bit_offset;
                  }
                }
              }
            }
          }
        }
      };

      this->search_thread = std::thread(search_loop);
      search_thread.detach();
    }

    template <typename ...Args>
    T* allocate(Args ...args) {
      T* ptr = nullptr;
      for (;;) {
        if (this->availptr) {
          this->last_availptr = this->availptr;
          ptr = this->availptr;
          this->availptr = nullptr;
          break;
        }
      }

      this->blkmap.set(static_cast<size_t>(ptr - this->membegin), false);

      if constexpr (std::is_fundamental_v<T>) {
        *ptr = {args...};
      } else {
        if (!sizeof...(args))
          ptr->T();
        else 
          ptr->T(std::forward(args...));
      }

      return ptr;
    }

    void deallocate(T *p) {
      for (;;) {
        if (this->availptr) {
          this->blkmap.set(static_cast<size_t>(p - this->membegin), true);
          if (p < this->availptr)
            this->availptr = p;
        }
      }
    }
};
*/

} // namespace utils

#endif  // HASHMAP_UTILS_MEMPOOL_HPP
