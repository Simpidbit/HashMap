#ifndef HASHMAP_UTILS___ITERATOR_HPP
#define HASHMAP_UTILS___ITERATOR_HPP




namespace utils {


template <typename T, typename PT>
class _iterator { ~_iterator() = delete; };

template <typename T, typename PT>
class _iterator<T*, PT> {
  protected:
    T* ptr = nullptr;

  public:
    _iterator() = default;
    _iterator(const _iterator &iter) {
      this->ptr = iter.ptr;
    }
    _iterator(_iterator &&iter) {
      this->ptr = iter.ptr;
      iter.ptr = nullptr;
    }

    inline size_t number() const noexcept {
      return reinterpret_cast<size_t>(this->ptr);
    }

    inline void point_to(T* nptr) {
      this->ptr = nptr;
    }


    T& operator*() const { return *(this->ptr); }
    T* operator->() const { return this->ptr; }


    PT& operator=(const _iterator &iter) {
      this->ptr = iter.ptr;
      return static_cast<PT&>(*this);
    }
    PT& operator=(_iterator &&iter) {
      this->ptr = iter.ptr;
      iter.ptr = nullptr;
      return static_cast<PT&>(*this);
    }

    // before
    PT operator++() {
      this->goback();
      return *dynamic_cast<PT*>(this);
    }

    // after
    PT operator++(int) {
      PT tmp(static_cast<const PT &>(*this));
      this->goback();
      return tmp;
    }

    // before
    PT operator--() {
      this->gofront();
      return *dynamic_cast<PT*>(this);
    }

    // after
    PT operator--(int) {
      PT tmp(static_cast<const PT&>(*this));
      this->gofront();
      return tmp;
    }

    PT& operator+=(size_t n) {
      this->goback(n);
      return static_cast<PT&>(*this);
    }

    PT& operator-=(size_t n) {
      this->gofront(n);
      return static_cast<PT&>(*this);
    }

    PT operator+(size_t n) {
      PT tmp(static_cast<const PT&>(*this));
      tmp.goback(n);
      return tmp;
    }

    PT operator-(size_t n) {
      PT tmp(static_cast<const PT&>(*this));
      tmp.gofront(n);
      return tmp;
    }

    virtual bool operator<(const _iterator &iter) {
      return (this->ptr < iter.ptr);
    }
    virtual bool operator==(const _iterator &iter) {
      return (this->ptr == iter.ptr);
    }
    virtual bool operator!=(const _iterator &iter) {
      return (this->ptr != iter.ptr);
    }
    virtual bool operator>(const _iterator &iter) {
      return (this->ptr == iter.ptr);
    }


    virtual void goback() = 0;
    virtual void goback(size_t n) = 0;
    virtual void gofront() = 0;
    virtual void gofront(size_t n) = 0;
};


} // namespace utils


#endif
