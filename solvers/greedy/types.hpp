#ifndef SRC_TYPES_HPP_
#define SRC_TYPES_HPP_

/**
 * @file types.hpp
 * @brief Header with base types:
 * Vec - a static vector for storing phsical quantities,
 * VecXd - a dynamic container for storing numbers and ready to be multiplied by matrices
 * Range - extended std::vector container with additional list access and set operators
 * @example types_test.cpp
 */

#include "includes.hpp"
#include "common.hpp"

/// @cond
#include "Eigen/Dense"
/// @endcond

namespace Eigen {
/**
 * Return iterator pointing to the begining of matrix.
 * Together with the end function this makes it possible to use matrices and vectors
 * in range for loops.
 */
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
typename Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Scalar* begin(
        Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v) {
    return v.data();
}
/**
 * Return const iterator pointing to the begining of matrix.
 * Together with the end function this makes it possible to use const matrices and vectors
 * in range for loops.
 */
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
typename Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Scalar* end(
        Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v) {
    return v.data() + v.size();
}
/**
 * Return iterator pointing to the end of matrix.
 * Together with the begin function this makes it possible to use const matrices and
 * vectors
 * in range for loops.
 */
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
const typename Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Scalar* begin(
    const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v) {
    return v.data();
}
/**
 * Return const iterator pointing to the end of matrix.
 * Together with the begin function this makes it possible to use const matrices and
 * vectors
 * in range for loops.
 */
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
const typename Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Scalar* end(
    const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v) {
    return v.data() + v.size();
}
}  // namespace Eigen

namespace std {  // make this also usable as std::begin and std::end
using Eigen::begin;
using Eigen::end;
}  // namespace std

namespace mm {

/// typedef for container for indexes
typedef std::vector<int> indexes_t;

/**
 * Wrapper around Eigen Vec class with our own constructors and operators.
 * This class uses Eigen's column vector and can therefore be freely multiplyed as A*x
 * by a matrix A.
 * It is used to represent a physical quantity, a velocity vector for example.
 * A sensible usage is up to 4 dimensions.
 **/
template <class Scalar, int dim>
class Vec : public Eigen::Matrix<Scalar, dim, 1> {
  private:
    typedef Eigen::Matrix<Scalar, dim, 1> matrix_t;  ///< Underlying storage type

  public:
    static const int dimension = dim;  ///< dimension of our vector
    typedef Scalar scalar_t;  ///< Underlying scalar type of vector
    /// Default constructor
    Vec() {}
    /// Vec(1)
    Vec(scalar_t val) { this->setConstant(val); }
    /// Vec({1, 2, 3})
    Vec(std::initializer_list<scalar_t> lst) {
        assert(lst.size() == dim &&
               "Initializer list length must match vector dimension in Vec constructor.");
        int i = 0;
        for (scalar_t x : lst) this->operator[](i++) = x;
    }
    /// allow default copy assignment
    Vec& operator=(const Vec&) = default;
    /// a = 4;
    void operator=(const scalar_t& x) {
        for (int i = 0; i < dim; ++i) this->operator[](i) = x;
    }
    /// a = {1, 3, 4};
    void operator=(std::initializer_list<scalar_t> lst) {
        assert(lst.size() == dim &&
               "Initializer list length must match vector dimension in Vec assignment.");
        int i = 0;
        for (scalar_t x : lst) this->operator[](i++) = x;
    }

    /// Return right hand side perpendicular vector in 2D, and some perpendivular vector otherwise.
    Vec getPerpendicular() const {
        static_assert(dim >= 2, "Cannot get perpendicular vector in 1d.");
        Vec p = 0.0;
        p[0] = this->operator[](1);
        p[1] = -this->operator[](0);
        return p;
    }

    /// Use inherited interface
    using matrix_t::matrix_t;  // bring in Matrix constructors
    using matrix_t::operator==;
    using matrix_t::operator!=;

    /// Returns dimension of a vector.
    static int size() { return dim; }

    /// Lexicographical compare of vectors
    bool operator<(const Vec& arg) const {
        for (int i = 0; i < dimension; ++i) {
            if (this->operator[](i) == arg[i]) continue;
            return this->operator[](i) < arg[i];
        }
        return false;
    }
    /// Lexicographical compare of vectors
    bool operator>(const Vec& arg) const { return arg < *this; }
    /// Lexicographical compare of vectors
    bool operator<=(const Vec& arg) const { return !(arg < *this); }
    /// Lexicographical compare of vectors
    bool operator>=(const Vec& arg) const { return !(*this < arg); }
    /// iterator to beginning
    Scalar* begin() { return Eigen::begin(*this); }
    /// const iterator to beginning
    const Scalar* begin() const { return Eigen::begin(*this); }
    /// iterator to end
    Scalar* end() { return Eigen::end(*this); }
    /// const iterator to end
    const Scalar* end() const { return Eigen::end(*this); }
    /// std operator << overload

    friend std::ostream& operator<<(std::ostream& xx, const Vec<Scalar, dim>& arr) {
        xx << "[";
        for (int i = 0; i < dim; i++) {
            if (i) xx << ",";
            xx << arr[i];
        }
        return xx << "]";
    }
};

/**
 * Vector type for arbitrarily large vectors, ie. things that can be multiplied by matrices.
 * This should be your goto container for numbers.
 **/
template <class Scalar>
class VecX : public Eigen::Matrix<Scalar, Eigen::Dynamic, 1> {
  private:
    typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> matrix_t;  ///< Underlying storage type

  public:
    typedef Scalar scalar_t;  ///< Underlying scalar type of vector

    /**
     * This class represents a non contiguous view to a VecX, allowing for
     * read, write and compare operations.
     */
    struct VecXView {
        VecX& receiver;  ///< reference to object we are viewing / modifying
        const indexes_t& modifier;  ///< list of indexes of elements to modify

        VecXView(VecXView&) = delete;  ///< Disallow copying
        VecXView(VecXView&&) = default;  ///< Allow moving
        VecXView& operator=(VecXView&) = delete;  ///< Disallow copying
        VecXView& operator=(VecXView&&) = default;  ///< Allow moving

        /// constructor
        VecXView(VecX& receiver_, const indexes_t& modifier_)
                : receiver(receiver_), modifier(modifier_) {}

        /// cast to underlying container
        operator VecX() const {
            VecX ret(size());
            int sz = modifier.size();
            for (int i = 0; i < sz; ++i) ret[i] = operator[](i);
            return ret;
        }

        /// a[{1, 2, 3}] = VecX<int>({1, 2, 3});
        void operator=(const VecX& rhs) {
            assert(rhs.size() == size() &&
                   "Container sizes must match in multi-indexed assignment.");
            for (int i = 0; i < size(); ++i) operator[](i) = rhs[i];
        }
        /// a = {1, 2, 3};
        void operator=(std::initializer_list<scalar_t> lst) {
            assert(lst.size() == size() &&
                   "Initializer list size must match container size in multi-indexed assignment.");
            int i = 0;
            for (const auto& x : lst) operator[](i++) = x;
        }
        /// a = 4;
        void operator=(const scalar_t& x) {
            for (int i = 0; i < size(); ++i)
                operator[](i) = x;
        }

        /// Write access to sub-container elements
        scalar_t& operator[](int i) { return receiver[modifier[i]]; }
        /// Read access to sub-container elements
        const scalar_t& operator[](int i) const { return receiver[modifier[i]]; }
        /// Size of the sub-container
        int size() const { return modifier.size(); }

        /// Compare contents of two views for equality.
        bool operator==(const VecXView& rhs) const {
            if (size() != rhs.size()) return false;
            for (int i = 0; i < size(); ++i) {
                if (operator[](i) != rhs[i]) return false;
            }
            return true;
        }
        /// True if contents of two views are different.
        bool operator!=(const VecXView& rhs) const { return !(*this == rhs); }

        /**
         * Returns list of indexes for which predicate returns true
         * Pred is any callable object, e.g., lambda func, functional,
         * class with operator () defined.
         */
        template <class Pred>
        indexes_t filter(const Pred& pred) const {
            indexes_t ret;
            for (int i = 0; i < size(); ++i)
                if (pred(operator[](i))) ret.push_back(i);
            return ret;
        }
        /// Returns list of indexes for which their elements compare less than a
        indexes_t operator<(const scalar_t& v) const {
            return filter([&](const scalar_t& t) { return t < v; });
        }
        /// Returns list of indexes for which their elements compare greater than a
        indexes_t operator>(const scalar_t& v) const {
            return filter([&](const scalar_t& t) { return t > v; });
        }
        /// Returns list of indexes for which their elements compare less or equal to a
        indexes_t operator<=(const scalar_t& v) const {
            return filter([&](const scalar_t& t) { return t <= v; });
        }
        /// Returns list of indexes for which their elements compare greater or equal to a
        indexes_t operator>=(const scalar_t& v) const {
            return filter([&](const scalar_t& t) { return t >= v; });
        }
        /// Returns list of indexes for which their elements compare equal to a
        indexes_t operator==(const scalar_t& v) const {
            return filter([&](const scalar_t& t) { return t == v; });
        }
        /// Returns list of indexes for which their elements compare not equal to a
        indexes_t operator!=(const scalar_t& v) const {
            return filter([&](const scalar_t& t) { return t != v; });
        }
    };

    /// Default constructor
    VecX() {}
    /// VecXd(size), uninitialized
    VecX(int size) : matrix_t::Matrix(size) {}
    /// VecXd(size, value)
    VecX(int size_, scalar_t val) : matrix_t::Matrix(size_) { this->setConstant(val); }
    /// VecXd({1, 2, 3})
    VecX(std::initializer_list<scalar_t> lst) {
        matrix_t::resize(lst.size());
        int i = 0;
        for (scalar_t x : lst) operator[](i++) = x;
    }
    /// a = {1, 3, 4};
    void operator=(std::initializer_list<scalar_t> lst) {
        this->resize(lst.size());
        int i = 0;
        for (scalar_t x : lst) operator[](i++) = x;
    }
    /// constructor from Eigen expressions
    template <typename OtherDerived>
    VecX(const Eigen::MatrixBase<OtherDerived>& m) : matrix_t::Matrix(m) {}

    /// Multi-indexed access for writing
    VecXView operator[](const indexes_t& indexes) {
        for (int idx : indexes)
            assert(0 <= idx && idx < size() &&
                   "One of indexes out of range when using multi-indexed write access.");
        return {*this, indexes};
    }
    /// Multi-indexed access for reading
    VecX operator[](const indexes_t& indexes) const {
        for (int idx : indexes)
            assert(0 <= idx && idx < size() &&
                   "One of indexes out of range when using multi-indexed read access.");
        VecX res(indexes.size());
        for (int idx : indexes) res[idx] = operator[](idx);
        return res;
    }

    /// Return size of the vector.
    int size() const { return matrix_t::size(); }

    /// Lexicographical compare of vectors
    bool operator<(const VecX& arg) const {
        for (int i = 0; i < this->size(); ++i) {
            if (this->operator[](i) == arg[i]) continue;
            return this->operator[](i) < arg[i];
        }
        return false;
    }
    /// Lexicographical compare of vectors
    bool operator>(const VecX& arg) const { return arg < *this; }
    /// Lexicographical compare of vectors
    bool operator<=(const VecX& arg) const { return !(arg < *this); }
    /// Lexicographical compare of vectors
    bool operator>=(const VecX& arg) const { return !(*this < arg); }
    /// iterator to beginning
    Scalar* begin() { return Eigen::begin(*this); }
    /// const iterator to beginning
    const Scalar* begin() const { return Eigen::begin(*this); }
    /// iterator to end
    Scalar* end() { return Eigen::end(*this); }
    /// const iterator to end
    const Scalar* end() const { return Eigen::end(*this); }
    /**
     * Returns list of indexes for which predicate returns true
     * Pred is any callable object, e.g., lambda func, functional,
     * class with operator () defined.
     */
    template <class Pred>
    indexes_t filter(const Pred& pred) const {
        indexes_t ret;
        for (int i = 0; i < size(); ++i)
            if (pred(this->operator[](i))) ret.push_back(i);
        return ret;
    }
    /// Returns list of indexes for which their elements compare less than a
    indexes_t operator<(const Scalar& v) const {
        return filter([&](const Scalar& t) { return t < v; });
    }
    /// Returns list of indexes for which their elements compare greater than a
    indexes_t operator>(const Scalar& v) const {
        return filter([&](const Scalar& t) { return t > v; });
    }
    /// Returns list of indexes for which their elements compare less or equal to a
    indexes_t operator<=(const Scalar& v) const {
        return filter([&](const Scalar& t) { return t <= v; });
    }
    /// Returns list of indexes for which their elements compare greater or equal to a
    indexes_t operator>=(const Scalar& v) const {
        return filter([&](const Scalar& t) { return t >= v; });
    }
    /// Returns list of indexes for which their elements compare equal to a
    indexes_t operator==(const Scalar& v) const {
        return filter([&](const Scalar& t) { return t == v; });
    }
    /// Returns list of indexes for which their elements compare not equal to a
    indexes_t operator!=(const Scalar& v) const {
        return filter([&](const Scalar& t) { return t != v; });
    }

    using matrix_t::operator[];
};

/// Bring name into scope.
template<typename T>
using VecXView = typename VecX<T>::VecXView;

/// Compare vector to a view
template <typename T>
bool operator==(const VecXView<T>& lhs, const VecX<T>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (int i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) return false;
    }
    return true;
}
/// Compare vector to a view
template <typename T>
bool operator==(const VecX<T>& lhs, const VecXView<T>& rhs) { return rhs == lhs; }
/// Compare vector to a view
template <typename T>
bool operator!=(const VecXView<T>& lhs, const VecX<T>& rhs) { return !(lhs == rhs); }
/// Compare vector to a view
template <typename T>
bool operator!=(const VecX<T>& lhs, const VecXView<T>& rhs) { return !(lhs == rhs); }

/**
 * @brief std::cout<< overload for  Matrix<Scalar,Dynamic,1>
 */
template <class Scalar>
std::ostream& operator<<(std::ostream& xx, const Eigen::Matrix<Scalar, Eigen::Dynamic, 1>& arr) {
    xx << "[";
    for (int i = 0; i < arr.size(); ++i) {
        xx << arr[i];
        if (i < arr.size() - 1) xx << ",";
    }
    xx << "]";
    return xx;
}

/// 1 dimensional vector of doubles
typedef Vec<double, 1> Vec1d;
/// 2 dimensional vector of doubles
typedef Vec<double, 2> Vec2d;
/// 3 dimensional vector of doubles
typedef Vec<double, 3> Vec3d;
/// X dimensional vector of doubles
typedef VecX<double> VecXd;

/**
 * @brief extension of std::vector with additional access operators
 * This is a general container, for example for Vec2d. For numeric values
 * and operators use VecXd.
 * @details see test_types.cpp for examples
 * @tparam T type of data inside of class
 */
template <class T>
class Range : public std::vector<T> {
  public:
    /// This container's value type
    typedef typename std::vector<T>::value_type value_type;
    /// This container's reference to value type
    typedef typename std::vector<T>::reference reference;
    /// This container's const reference to value type
    typedef typename std::vector<T>::const_reference const_reference;
    /// This container's size type
    typedef int size_type;

    /**
     * This class represents a non contiguous view to a Range, allowing for
     * read, write and compare operations.
     */
    struct RangeView {
        Range<T>& receiver;  ///< reference to object we are viewing / modifying
        const indexes_t& modifier;  ///< list of indexes of elements to modify

        RangeView(RangeView&) = delete;  ///< Disallow copying
        RangeView(RangeView&&) = default;  ///< Allow moving
        RangeView& operator=(RangeView&) = delete;  ///< Disallow copying
        RangeView& operator=(RangeView&&) = default;  ///< Allow moving

        /// constructor
        RangeView(Range<T>& receiver_, const indexes_t& modifier_)
                : receiver(receiver_), modifier(modifier_) {}

        /// cast to underlying container
        operator Range<T>() const {
            Range<T> ret(size());
            int sz = modifier.size();
            for (int i = 0; i < sz; ++i) ret[i] = operator[](i);
            return ret;
        }

        /// a[{1, 2, 3}] = Range<int>({1, 2, 3});
        void operator=(const Range<T>& rhs) {
            assert(rhs.size() == size() &&
                   "Container sizes must match in multi-indexed assignment.");
            for (size_type i = 0; i < size(); ++i) operator[](i) = rhs[i];
        }
        /// a = {1, 2, 3};
        void operator=(std::initializer_list<value_type> lst) {
            assert(static_cast<int>(lst.size()) == size() &&
                   "Initializer list size must match container size in multi-indexed assignment.");
            int i = 0;
            for (const_reference x : lst) operator[](i++) = x;
        }
        /// a = 4;
        void operator=(const value_type& x) {
            for (size_type i = 0; i < size(); ++i)
                operator[](i) = x;
        }

        /// Write access to sub-container elements
        reference operator[](size_type i) { return receiver[modifier[i]]; }
        /// Read access to sub-container elements
        const_reference operator[](size_type i) const { return receiver[modifier[i]]; }
        /// Size of the sub-container
        size_type size() const { return static_cast<int>(modifier.size()); }

        /// Remove elements in this sub-container from the container
        void remove() {
            Range<int> tmp(modifier);
            std::sort(tmp.begin(), tmp.end());
            tmp.resize(std::unique(tmp.begin(), tmp.end()) - tmp.begin());
            auto it = receiver.begin();
            auto cur = receiver.begin();
            int to_remove = 0;
            int c = 0;
            while (cur != receiver.end()) {
                if (to_remove < tmp.size() && c == tmp[to_remove]) {
                    ++to_remove;
                } else {
                    *it++ = std::move(*cur);
                }
                ++c;
                ++cur;
            }
            receiver.resize(it - receiver.begin());
        }

        /// Compare contents of two views for equality.
        bool operator==(const RangeView& rhs) const {
            if (size() != rhs.size()) return false;
            for (int i = 0; i < size(); ++i) {
                if (operator[](i) != rhs[i]) return false;
            }
            return true;
        }
        /// True if contents of two views are different.
        bool operator!=(const RangeView& rhs) const { return !(*this == rhs); }

        /**
         * Returns list of indexes for which predicate returns true
         * Pred is any callable object, e.g., lambda func, functional,
         * class with operator () defined.
         */
        template <class Pred>
        indexes_t filter(const Pred& pred) const {
            indexes_t ret;
            for (size_type i = 0; i < size(); ++i)
                if (pred(operator[](i))) ret.push_back(i);
            return ret;
        }
        /// Returns list of indexes for which their elements compare less than a
        indexes_t operator<(const value_type& v) const {
            return filter([&](const value_type& t) { return t < v; });
        }
        /// Returns list of indexes for which their elements compare greater than a
        indexes_t operator>(const value_type& v) const {
            return filter([&](const value_type& t) { return t > v; });
        }
        /// Returns list of indexes for which their elements compare less or equal to a
        indexes_t operator<=(const value_type& v) const {
            return filter([&](const value_type& t) { return t <= v; });
        }
        /// Returns list of indexes for which their elements compare greater or equal to a
        indexes_t operator>=(const value_type& v) const {
            return filter([&](const value_type& t) { return t >= v; });
        }
        /// Returns list of indexes for which their elements compare equal to a
        indexes_t operator==(const value_type& v) const {
            return filter([&](const value_type& t) { return t == v; });
        }
        /// Returns list of indexes for which their elements compare not equal to a
        indexes_t operator!=(const value_type& v) const {
            return filter([&](const value_type& t) { return t != v; });
        }
    };
  public:
    /// Default constructor
    Range<T>() {}
    /// Copy constructor for vector
    Range<T>(const std::vector<T>& v) : std::vector<T>(v) {}
    /// Construct Range from Vec
    template<int dim>
    Range<T>(const Vec<T, dim>& v) : std::vector<T>(std::begin(v), std::end(v)) {}
    /// Construct Range from VecX
    Range<T>(const VecX<T>& v) : std::vector<T>(std::begin(v), std::end(v)) {}
    /// Move constructor for vector
    Range<T>(std::vector<T>&& v) : std::vector<T>(std::move(v)) {}
    Range<T>(const Range<T>&) = default;  ///< explicitly default all constructors
    Range<T>(Range<T>&&) = default;  ///< explicitly default all constructors
    Range<T>& operator=(const Range<T>&) = default;  ///< explicitly default all assignments
    Range<T>& operator=(Range<T>&&) = default;  ///< explicitly default all assignments
    /// Assignment for vector
    Range<T>& operator=(const std::vector<T>& v) {
        std::vector<T>::operator=(v);
        return *this;
    }
    /// a = {1, 2, 3};
    Range& operator=(std::initializer_list<value_type> lst) {
        this->resize(lst.size());
        int i = 0;
        for (const_reference x : lst) operator[](i++) = x;
        return *this;
    }
    /// a = 4;
    void operator=(const value_type& x) {
        for (size_type i = 0; i < size(); ++i)
            operator[](i) = x;
    }
    /// Move assignment for vector
    Range<T>& operator=(std::vector<T>&& v) {
        std::vector<T>::operator=(std::move(v));
        return *this;
    }
    /// overload vector's [] to assert parameter
    reference operator[](size_type x) {
        assert(0 <= x && x < static_cast<int>(size()) &&
               "Index out of range when trying to access Range for write.");
        return std::vector<T>::operator[](x);
    }
    /// overload vector's [] to assert parameter
    const_reference operator[](size_type x) const {
        assert(0 <= x && x < static_cast<int>(size()) &&
               "Index out of range when trying to access Range for read.");
        return std::vector<T>::operator[](x);
    }
    /// Multi-indexed access for writing
    RangeView operator[](const indexes_t& indexes) {
        for (size_type idx : indexes)
            assert(0 <= idx && idx < static_cast<int>(size()) &&
                   "One of indexes out of range when using multi-indexed write access.");
        return {*this, indexes};
    }
    /// Multi-indexed access for reading
    Range<T> operator[](const indexes_t& indexes) const {
        for (size_type idx : indexes)
            assert(0 <= idx && idx < static_cast<int>(size()) &&
                   "One of indexes out of range when using multi-indexed read access.");
        Range<T> res;
        res.reserve(indexes.size());
        for (size_type idx : indexes) res.push_back(operator[](idx));
        return res;
    }

    /// returns size of a Range
    int size() const { return static_cast<int>(std::vector<T>::size()); }

    /// Append all elements of `rng` to self.
    void append(const Range<T>& rng) {
        this->insert(this->end(), rng.begin(), rng.end());
    }
    /**
    * @brief Return new copy containing this range's elements followed by all
    * elements of rng.
    */
    Range<T> join(const Range<T>& rng) const {
        Range<T> ret;
        ret = *this;
        ret.insert(ret.end(), rng.begin(), rng.end());
        return ret;
    }

    /**
     * Returns list of indexes for which predicate returns true
     * Pred is any callable object, e.g., lambda func, functional,
     * class with operator () defined.
     * Example:
     * @code
     * a[a.filter([](double v){return v>2.3 && v<6.4;})]
     * @endcode
     * returns all elements of a that are within 2.3 and 6.4.
     */
    template <class Pred>
    indexes_t filter(const Pred& pred) const {
        indexes_t ret;
        for (size_type i = 0; i < size(); ++i)
            if (pred(operator[](i))) ret.push_back(i);
        return ret;
    }
    /// Returns list of indexes for which their elements compare less than a
    indexes_t operator<(const value_type& v) const {
        return filter([&](const value_type& t) { return t < v; });
    }
    /// Returns list of indexes for which their elements compare greater than a
    indexes_t operator>(const value_type& v) const {
        return filter([&](const value_type& t) { return t > v; });
    }
    /// Returns list of indexes for which their elements compare less or equal to a
    indexes_t operator<=(const value_type& v) const {
        return filter([&](const value_type& t) { return t <= v; });
    }
    /// Returns list of indexes for which their elements compare greater or equal to a
    indexes_t operator>=(const value_type& v) const {
        return filter([&](const value_type& t) { return t >= v; });
    }
    /// Returns list of indexes for which their elements compare equal to a
    indexes_t operator==(const value_type& v) const {
        return filter([&](const value_type& t) { return t == v; });
    }
    /// Returns list of indexes for which their elements compare not equal to a
    indexes_t operator!=(const value_type& v) const {
        return filter([&](const value_type& t) { return t != v; });
    }

    using std::vector<T>::vector;
};

/// Bring name into scope.
template<typename T>
using RangeView = typename Range<T>::RangeView;

/// Compare vector to a view
template <typename T>
bool operator==(const RangeView<T>& lhs, const Range<T>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (int i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) return false;
    }
    return true;
}
/// Compare vector to a view
template <typename T>
bool operator==(const Range<T>& lhs, const RangeView<T>& rhs) { return rhs == lhs; }
/// Compare vector to a view
template <typename T>
bool operator!=(const RangeView<T>& lhs, const Range<T>& rhs) { return !(lhs == rhs); }
/// Compare vector to a view
template <typename T>
bool operator!=(const Range<T>& lhs, const RangeView<T>& rhs) { return !(lhs == rhs); }

/**
* @brief Return new Range containing all elements of A followed by all
* elements of B.
*/
template <class T>
Range<T> join(const Range<T>& a, const Range<T>& b) {
    Range<T> ret = a;
    ret.insert(ret.end(), b.begin(), b.end());
    return ret;
}

/// print formatted for vectors -- recursive
template <typename scalar_t>
std::ostream& print_formatted(const VecX<scalar_t>& v, const std::string& before = "{",
                              const std::string& delimiter = ", ",
                              const std::string& after = "}",
                              const std::string finish = ";", std::ostream& xx = std::cout) {
    bool first = true;
    xx << before;
    for (const scalar_t& x : v) {
        if (!first) xx << delimiter;
        first = false;
        print_formatted(x, before, delimiter, after, "", xx);
    }
    return xx << after << finish;
}
/// print formatted for vectors -- recursive
template <typename scalar_t, int dim>
std::ostream& print_formatted(const Vec<scalar_t, dim>& v, const std::string& before = "{",
                              const std::string& delimiter = ", ",
                              const std::string& after = "}",
                              const std::string finish = ";", std::ostream& xx = std::cout) {
    bool first = true;
    xx << before;
    for (const scalar_t& x : v) {
        if (!first) xx << delimiter;
        first = false;
        print_formatted(x, before, delimiter, after, "", xx);
    }
    return xx << after << finish;
}

}  // namespace mm

#endif  // SRC_TYPES_HPP_
