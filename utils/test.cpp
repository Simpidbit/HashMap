#include <algorithm>
#include <functional>
#include <iostream>
#include <ranges>
#include <vector>

#include <array>
#include <cassert>

template <typename E>
struct vector_expr {
  size_t size() const { return static_cast<const E*>(this)->size(); }
  decltype(auto) operator[](size_t i) const { return static_cast<const E*>(this)->operator[](i); }
};

template <typename T>
class vector : public std::vector<T>, public vector_expr<vector<T> > {
public:
	using std::vector<T>::vector;
	using std::vector<T>::size; 
	using std::vector<T>::operator[];

  template <typename E>
  vector(const vector_expr<E> &expr) {
    this->resize(expr.size());
    for (size_t i = 0; i < expr.size(); i++)
      this->operator[](i) = expr[i];
  }
  
	template <typename E>
	vector<T>& operator=(const E &e) {
		const auto count = std::min(size(), e.size());
		this->resize(count);
		for (std::size_t idx{0}; idx < count; ++idx) {
			this->operator[](idx) = e[idx];
		}
		return *this;
	}


};

template <typename E1, typename E2>
struct vector_sum : public vector_expr<vector_sum<E1, E2> > {
  const E1 e1;
  const E2 e2;

  vector_sum(const E1 &e1, const E2 &e2) 
    : e1(e1), e2(e2) {}

  decltype(auto) operator[] (size_t i) const {
    return this->e1[i] + this->e2[i];
  }

  size_t size() const { return this->e2.size(); };
};

template <typename E1, typename E2>
vector_sum<E1, E2>
operator+(const vector_expr<E1> &e1, const vector_expr<E2> &e2) {
  return vector_sum<E1, E2>(e1, e2);
}


template <typename E1, typename E2>
struct vector_minu : public vector_expr<vector_minu<E1, E2> > {
  const E1 e1;
  const E2 e2;

  vector_minu(const E1 &e1, const E2 &e2) 
    : e1(e1), e2(e2) {}

  decltype(auto) operator[] (size_t i) const {
    return this->e1[i] - this->e2[i];
  }

  size_t size() const { return this->e2.size(); };
};

template <typename E1, typename E2>
vector_minu<E1, E2>
operator-(const vector_expr<E1> &e1, const vector_expr<E2> &e2) {
  return vector_minu<E1, E2>(e1, e2);
}


template <typename E1, typename E2>
struct vector_multi : public vector_expr<vector_multi<E1, E2> > {
  const E1 e1;
  const E2 e2;

  vector_multi(const E1 &e1, const E2 &e2) 
    : e1(e1), e2(e2) {}

  decltype(auto) operator[] (size_t i) const {
    return this->e1[i] * this->e2[i];
  }

  size_t size() const { return this->e2.size(); };
};

template <typename E1, typename E2>
vector_multi<E1, E2>
operator*(const vector_expr<E1> &e1, const vector_expr<E2> &e2) {
  return vector_multi<E1, E2>(e1, e2);
}

template <typename E1, typename E2>
struct vector_divide : public vector_expr<vector_divide<E1, E2> > {
  const E1 e1;
  const E2 e2;

  vector_divide(const E1 &e1, const E2 &e2) 
    : e1(e1), e2(e2) {}

  decltype(auto) operator[] (size_t i) const {
    return this->e1[i] / this->e2[i];
  }

  size_t size() const { return this->e2.size(); };
};

template <typename E1, typename E2>
vector_divide<E1, E2>
operator/(const vector_expr<E1> &e1, const vector_expr<E2> &e2) {
  return vector_divide<E1, E2>(e1, e2);
}



// operator+
// operator-
// operator*
// operator/

/*
!!! You should Not modify the following codes !!!
*/

void print(vector<double> vec) {
	for (auto&& v: vec) {
		std::cout << v << " ";
	}
	std::cout << std::endl;
}

int main() {
	const vector<double> a { 1.2764, 1.3536, 1.2806, 1.9124, 1.8871, 1.7455};
	const vector<double> b { 2.1258, 2.9679, 2.7635, 2.3796, 2.4820, 2.4195};
	const vector<double> c { 3.9064, 3.7327, 3.4760, 3.5705, 3.8394, 3.8993};
	const vector<double> d { 4.7337, 4.5371, 4.5517, 4.2110, 4.6760, 4.3139};
	const vector<double> e { 5.2126, 5.1452, 5.8678, 5.1879, 5.8816, 5.6282};
  
  {
		std::cout << "Standard outputs:\n";
		vector<double> result(6);
		for (std::size_t idx = 0; idx < 6; idx++) {
			result[idx] = a[idx] - b[idx] * c[idx] / d[idx] + e[idx];
      //result[idx] = a[idx] + b[idx];
		}
		print(result);
	}
	{
		std::cout << "Your outputs:\n";
		vector<double> result(6);
		result = a - b * c / d + e; // use the expression template to calculate.
    //result = a + b;
		print(result);
	}
	return 0;
}
/*
template <typename E>
class VecExpression {
  public:
    static constexpr bool is_leaf = false;

    double operator[](size_t i) const {
        // Delegation to the actual expression type. This avoids dynamic polymorphism (a.k.a. virtual functions in C++)
        return static_cast<E const&>(*this)[i];
    }
    size_t size() const { return static_cast<E const&>(*this).size(); }
};

class Vec : public VecExpression<Vec> {
    std::array<double, 3> elems;

  public:
    static constexpr bool is_leaf = true;

    decltype(auto) operator[](size_t i) const { return elems[i]; }
    //decltype(auto) &operator[](size_t i)      { return elems[i]; }
    size_t size()               const { return elems.size(); }

    // construct Vec using initializer list 
    Vec(std::initializer_list<double> init) {
        std::copy(init.begin(), init.end(), elems.begin());
    }

    // A Vec can be constructed from any VecExpression, forcing its evaluation.
    template <typename E>
    Vec(VecExpression<E> const& expr) {
        for (size_t i = 0; i != expr.size(); ++i) {
            elems[i] = expr[i];
        }
    }
};

template <typename E1, typename E2>
class VecSum : public VecExpression<VecSum<E1, E2> > {
  // cref if leaf, copy otherwise
  typename std::conditional<E1::is_leaf, const E1&, const E1>::type _u;
  typename std::conditional<E2::is_leaf, const E2&, const E2>::type _v;

  public:
    static constexpr bool is_leaf = false;

    VecSum(E1 const& u, E2 const& v) : _u(u), _v(v) {
        assert(u.size() == v.size());
    }
    decltype(auto) operator[](size_t i) const { return _u[i] + _v[i]; }
    size_t size()               const { return _v.size(); }
};
  
template <typename E1, typename E2>
VecSum<E1, E2>
operator+(VecExpression<E1> const& u, VecExpression<E2> const& v) {
   return VecSum<E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E2*>(&v));
}

int main() {
    Vec v0 = {23.4,  12.5,  144.56};
    Vec v1 = {67.12, 34.8,  90.34};
    Vec v2 = {34.90, 111.9, 45.12};
    
    // Following assignment will call the ctor of Vec which accept type of 
    // `VecExpression<E> const&`. Then expand the loop body to 
    // a.elems[i] + b.elems[i] + c.elems[i]
    Vec sum_of_vec_type = v0 + v1 + v2; 

    for (size_t i = 0; i < sum_of_vec_type.size(); ++i)
        std::cout << sum_of_vec_type[i] << std::endl;

    // To avoid creating any extra storage, other than v0, v1, v2
    // one can do the following (Tested with C++11 on GCC 5.3.0)
    auto sum = v0 + v1 + v2;
    for (size_t i = 0; i < sum.size(); ++i)
        std::cout << sum[i] << std::endl;
    // Observe that in this case typeid(sum) will be VecSum<VecSum<Vec, Vec>, Vec>
    // and this chaining of operations can go on.
}
*/
