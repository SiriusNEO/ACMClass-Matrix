#ifndef SJTU_MATRIX_HPP
#define SJTU_MATRIX_HPP

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <iterator>

using std::size_t;

namespace sjtu
{
    template <class T>
    class Matrix
    {
    private:
        // your private member variables here.
        T *element;
        size_t rowN, columnN;
        template <class> friend class Matrix;

    public:
        Matrix() {
            element = nullptr;
            rowN = columnN = 0;
        }

        Matrix(size_t n, size_t m, T _init = T())
        {
            rowN = n, columnN = m;
            element = new T [rowN*columnN];
            for (int i = 0; i < rowN*columnN; ++i) *(element+i) = T(_init);
        }

        explicit Matrix(std::pair<size_t, size_t> sz, T _init = T())
        {
            rowN = sz.first, columnN = sz.second;
            element = new T [rowN*columnN];
            for (int i = 0; i < rowN*columnN; ++i) *(element+i) = T(_init);
        }

        Matrix(const Matrix &o)
        {
            rowN = o.rowN, columnN = o.columnN;
            element = new T [rowN*columnN];
            for (int i = 0; i < rowN*columnN; ++i) *(element+i) = T(*(o.element+i));
        }

        template <class U>
        Matrix(const Matrix<U> &o)
        {
            rowN = o.rowN, columnN = o.columnN;
            element = new T [rowN*columnN];
            for (int i = 0; i < rowN*columnN; ++i) *(element+i) = T(*(o.element+i));
        }

        Matrix &operator=(const Matrix &o)
        {
            if (element == o.element) return *this;
            if (element) delete [] element, element = nullptr;
            rowN = o.rowN, columnN = o.columnN;
            element = new T [rowN*columnN];
            for (int i = 0; i < rowN*columnN; ++i) *(element+i) = T(*(o.element+i));
            return *this;
        }

        template <class U>
        Matrix &operator=(const Matrix<U> &o)
        {
            if ((void*)element == (void*)o.element) return *this;
            if (element) delete [] element, element = nullptr;
            rowN = o.rowN, columnN = o.columnN;
            element = new T [rowN*columnN];
            for (int i = 0; i < rowN*columnN; ++i) *(element+i) = T(*(o.element+i));
            return *this;
        }

        Matrix(Matrix &&o) noexcept:rowN(o.rowN), columnN(o.columnN), element(o.element)
        {
            o.element = nullptr;
        }

        Matrix &operator=(Matrix &&o) noexcept
        {
            if (element == o.element) return *this;
            if (element) delete [] element, element = nullptr;
            rowN = o.rowN, columnN = o.columnN, element = o.element;
            o.element = nullptr;
            return *this;
        }

        ~Matrix() {
            if (element) delete [] element, element = nullptr;
        }

        Matrix(std::initializer_list<std::initializer_list<T>> il)
        {
            for (int i = 1; i < il.size(); ++i)
                if ((*(il.begin()+i)).size() != (*(il.begin()+i-1)).size())
                    throw std::invalid_argument("Init by initializer_list Invalid");
            rowN = il.size(), columnN = (*il.begin()).size();
            element = new T [rowN*columnN];
            int cnt = 0;
            for (auto i : il) for (auto j : i) *(element+cnt) = j, ++cnt;
        }

    public:
        size_t rowLength() const { return rowN;}

        size_t columnLength() const { return columnN;}

        void resize(size_t _n, size_t _m, T _init = T())
        {
            if(rowN * columnN < _n*_m) {
                Matrix tmp = *this;
                delete [] element;
                element = new T [_n * _m];
                for (int i = 0; i < _n*_m; ++i) {
                    if (i < rowN * columnN) *(element+i) = *(tmp.element+i);
                    else *(element+i) = _init;
                }
                rowN = _n, columnN = _m;
            }
            else if (rowN * columnN >= _n * _m) {rowN = _n, columnN = _m;}
        }

        void resize(std::pair<size_t, size_t> sz, T _init = T())
        {
            resize(sz.first, sz.second, _init);
        }

        std::pair<size_t, size_t> size() const
        {
            return std::make_pair(rowN, columnN);
        };

        void clear()
        {
            if (element) delete [] element, element = nullptr;
            rowN = columnN = 0;
        }

    public:
        const T &operator()(size_t i, size_t j) const {return *(element+i*columnN+j);}

        T &operator()(size_t i, size_t j) {
            if (i < 0 || j < 0 || i >= rowN || j >= columnN) throw std::invalid_argument("(i, j) Invalid");
            return *(element+i*columnN+j);
        }

        Matrix<T> row(size_t i) const
        {
            if (i < 0 || i >= rowN) throw std::invalid_argument("row Invalid");
            Matrix ret(1, columnN, 0);
            for (int j = 0; j < columnN; ++j) ret(0, j) = (*this)(i, j);
            return ret;
        }

        Matrix<T> column(size_t i) const
        {
            if (i < 0 || i >= columnN) throw std::invalid_argument("column Invalid");
            Matrix ret(rowN, 1, 0);
            for (int j = 0; j < rowN; ++j) ret(j, 0) = (*this)(j, i);
            return ret;
        }

    public:
        template <class V, class U>
        friend auto operator*(const Matrix<V> &mat, const U &x);
        template <class V, class U>
        friend auto operator*(const U &x, const Matrix<V> &mat);
        template <class U, class V>
        friend auto operator*(const Matrix<U> &a, const Matrix<V> &b);
        template <class U, class V>
        friend auto operator+(const Matrix<U> &a, const Matrix<V> &b);
        template <class U, class V>
        friend  auto operator-(const Matrix<U> &a, const Matrix<V> &b);

        template <class U>
        bool operator==(const Matrix<U> &o) const
        {
            if (rowN != o.rowN || columnN != o.columnN) return false;
            for (int i = 0; i < rowN * columnN; ++i)
                if (*(element+i) != *(o.element+i)) return false;
            return true;
        }

        template <class U>
        bool operator!=(const Matrix<U> &o) const {return !((*this)==o);}

        Matrix operator-() const
        {
            Matrix ret(rowN, columnN, 0);
            for (int i = 0; i < rowN * columnN; ++i) *(ret.element+i) = -(*(element+i));
            return ret;
        }

        template <class U>
        Matrix &operator+=(const Matrix<U> &o) {*this=*this+o; return *this;}

        template <class U>
        Matrix &operator-=(const Matrix<U> &o) {return *this+=-o;}

        template <class U>
        Matrix &operator*=(const U &x) {*this=*this*x;return *this;}

        Matrix tran() const
        {
            Matrix ret(columnN, rowN, 0);
            for (int i = 0; i < rowN; ++i)
                for (int j = 0; j < columnN; ++j)
                    ret(j, i) = (*this)(i, j);
            return ret;
        }

    public: // iterator
        class iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type        = T;
            using pointer           = T *;
            using reference         = T &;
            using size_type         = size_t;
            using difference_type   = std::ptrdiff_t;

            iterator() = default;

            iterator(const iterator &) = default;

            iterator &operator=(const iterator &) = default;

            friend Matrix;

        public:
            pointer fstptr;
            std::pair<difference_type, difference_type> st, now, l, r;

        public:
            difference_type operator-(const iterator &o) {
                return (now.first*(r.second-l.second+1)+now.second - (o.now.first*(r.second-l.second+1)+o.now.second));
            }

            iterator &operator+=(difference_type offset) {while (offset--) ++(*this); return *this;}

            iterator operator+(difference_type offset) const {
                iterator ret = (*this); ret += offset; return ret;
            }

            iterator &operator-=(difference_type offset) {return *this+=(-offset);}

            iterator operator-(difference_type offset) const {while (offset--) --(*this); return *this;}

            iterator &operator++() {
                if (now.second == r.second) now.first++, now.second= l.second;
                else now.second++;
                return *this;
            }

            iterator operator++(int) {iterator ret = *this; ++(*this); return ret;}

            iterator &operator--() {
                if (now.second == l.second) now.first--, now.second = r.second;
                else now.second--;
            }

            iterator operator--(int) {iterator ret = *this; --(*this); return ret;}

            reference operator*() const {return *(fstptr+now.first*st.second+now.second);}

            pointer operator->() const {return fstptr+now.first*st.second+now.second;}

            bool operator==(const iterator &o) const {return (now.first==o.now.first)&&(now.second==o.now.second);}

            bool operator!=(const iterator &o) const {return !((*this)==o);}
        };

        iterator begin() {
            iterator ret;
            ret.fstptr = element;
            ret.st = std::make_pair(rowN, columnN);
            ret.now = ret.l = std::make_pair(0, 0);
            ret.r = std::make_pair(rowN-1, columnN-1);
            return ret;
        }

        iterator end() {
            iterator ret;
            ret.fstptr = element;
            ret.st = std::make_pair(rowN, columnN);
            ret.l = std::make_pair(0, 0);
            ret.now = ret.r = std::make_pair(rowN-1, columnN-1);
            return ret;
        }

        std::pair<iterator, iterator> subMatrix(std::pair<size_t, size_t> l, std::pair<size_t, size_t> r)
        {
            if (l.first < 0 || l.second < 0 || r.first < 0 || r.second < 0 || l.first >= rowN || l.second >= columnN || r.first >= rowN || r.second >= columnN)
                throw std::invalid_argument("subMatrix Invalid");
            iterator beg, end;
            beg.fstptr = end.fstptr = element;
            beg.st = end.st =  std::make_pair(rowN, columnN);
            beg.now = beg.l = end.l = l;
            end.now = beg.r = end.r = r;
            return std::make_pair(beg, end);
        }
    };
}

//
namespace sjtu
{
    template <class V, class U>
    auto operator*(const Matrix<V> &mat, const U &x)
    {
        V tmpV; U tmpU; Matrix<decltype(tmpU*tmpV)> ret(mat.rowN, mat.columnN, 0);
        for (int i = 0; i < mat.rowN * mat.columnN; ++i) *(ret.element+i) = *(mat.element+i) * x;
        return ret;
    }

    template <class V, class U>
    auto operator*(const U &x, const Matrix<V> &mat) {return mat*x;}

    template <class U, class V>
    auto operator*(const Matrix<U> &a, const Matrix<V> &b)
    {
        if (a.columnN != b.rowN) throw std::invalid_argument("Multiply Invalid");
        V tmpV; U tmpU; Matrix<decltype(tmpU*tmpV)> ret(a.rowN, b.columnN, 0);
        for (int i = 0; i < a.rowN; ++i)
            for (int j = 0; j < b.columnN; ++j)
                for (int k = 0; k < a.columnN; ++k)
                    ret(i, j) += a(i, k) * b(k, j);
        return ret;
    }

    template <class U, class V>
    auto operator+(const Matrix<U> &a, const Matrix<V> &b)
    {
        if (a.rowN != b.rowN || a.columnN != b.columnN) throw std::invalid_argument("Add Invalid");
        U tmpU; V tmpV; Matrix<decltype(tmpU+tmpV)> ret(a.rowN, a.columnN, 0);
        for (int i = 0; i < a.rowN * a.columnN; ++i) *(ret.element+i) = *(a.element+i) + *(b.element+i);
        return ret;
    }

    template <class U, class V>
    auto operator-(const Matrix<U> &a, const Matrix<V> &b)
    {
        return a+(-b);
    }
}

#endif //SJTU_MATRIX_HPP

