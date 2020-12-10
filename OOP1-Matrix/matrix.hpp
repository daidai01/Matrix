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
        template <class V,class U>
        friend auto operator*(const Matrix<V> &mat, const U &x);
        template <class V, class U>
        friend auto operator*(const U &x, const Matrix<V> &mat);
        template <class U, class V>
        friend auto operator*(const Matrix<U> &a, const Matrix<V> &b);
        template <class U, class V>
        friend auto operator+(const Matrix<U> &a, const Matrix<V> &b);
        template <class U, class V>
        friend auto operator-(const Matrix<U> &a, const Matrix<V> &b);

    private:
        // your private member variables here.
        size_t n=0,m=0,m_size=0;
        T *matrix=nullptr;

    public:
        Matrix() = default;

        Matrix(size_t _n, size_t _m, T _init = T())
        {
            n=_n;
            m=_m;
            m_size=n*m;
            matrix=new T[m_size];
            for(size_t i=0;i<m_size;++i) matrix[i]=_init;
        }

        explicit Matrix(std::pair<size_t, size_t> sz, T _init = T())
        {
            n=sz.first;
            m=sz.second;
            m_size=n*m;
            matrix=new T[m_size];
            for(size_t i=0;i<m_size;++i) matrix[i]=_init;
        }

        Matrix(const Matrix &o)//拷贝构造
        {
            n=o.n;
            m=o.m;
            m_size=o.m_size;
            matrix=new T[m_size];
            for(size_t i=0;i<m_size;++i) matrix[i]=o.matrix[i];
        }

        template <class U>
        explicit Matrix(const Matrix<U> &o)//自己加的explicit
        {
            n=o.rowLength();
            m=o.columnLength();
            m_size=n*m;
            matrix=new T[m_size];
            for(size_t i=0;i<m_size;++i)
                matrix[i]=o(i/o.columnLength(),i%o.columnLength());
        }

        Matrix &operator=(const Matrix &o)//赋值构造
        {
            if(this==&o) return *this;
            delete []matrix;
            n=o.n;
            m=o.m;
            m_size=o.m_size;
            matrix=new T[m_size];
            for(size_t i=0;i<m_size;++i) matrix[i]=o.matrix[i];
            return *this;
        }

        template <class U>
        Matrix &operator=(const Matrix<U> &o)
        {
            delete []matrix;
            n=o.rowLength();
            m=o.columnLength();
            m_size=n*m;
            matrix=new T[m_size];
            for(size_t i=0;i<m_size;++i)
                matrix[i]=o(i/o.columnLength(),i%o.columnLength());
            return *this;
        }

        Matrix(Matrix &&o) noexcept//移动构造
        {
            n=o.n;
            m=o.m;
            m_size=o.m_size;
            matrix=o.matrix;
            o.matrix=nullptr;
        }

        Matrix &operator=(Matrix &&o) noexcept//移动赋值
        {
            if(this==&o) return *this;
            delete []matrix;
            n=o.n;
            m=o.m;
            m_size=o.m_size;
            matrix=o.matrix;
            o.matrix=nullptr;
            return *this;
        }

        ~Matrix() {clear();}

        Matrix(std::initializer_list<std::initializer_list<T>> il)
        {
            n=il.size();
            m=il.begin()->size();
            m_size=n*m;
            matrix=new T[m_size];
            size_t cnt=0;
            for(auto it=il.begin();it!=il.end();++it)
            {
                if(it->size()!=m)
                {
                    delete []matrix;
                    throw std::invalid_argument("build failed");
                }
                for(auto iter=it->begin();iter!=it->end();++iter)
                {
                    matrix[cnt]=*iter;
                    cnt++;
                }
            }
        }

    public:
        size_t rowLength() const{return n;}

        size_t columnLength() const{return m;}

        void resize(size_t _n, size_t _m, T _init = T())
        {
            size_t old_size=m_size;
            n=_n;
            m=_m;
            m_size=n*m;
            if(old_size==m_size) return;
            T *tmp=matrix;
            matrix=new T[n*m];
            if(old_size>m_size)
                for(size_t i=0;i<m_size;++i) matrix[i]=tmp[i];
            else
            {
                for(size_t i=0;i<old_size;++i) matrix[i]=tmp[i];
                for(size_t i=old_size;i<m_size;++i) matrix[i]=_init;
            }
            delete []tmp;
        }

        void resize(std::pair<size_t, size_t> sz, T _init = T())
        {
            size_t old_size=m_size;
            n=sz.first;
            m=sz.second;
            m_size=n*m;
            if(old_size==m_size) return;
            T *tmp=matrix;
            matrix=new T[n*m];
            if(old_size>m_size)
                for(size_t i=0;i<m_size;++i) matrix[i]=tmp[i];
            else
            {
                for(size_t i=0;i<old_size;++i) matrix[i]=tmp[i];
                for(size_t i=old_size;i<m_size;++i) matrix[i]=_init;
            }
            delete []tmp;
        }

        std::pair<size_t, size_t> size() const
        {
            return std::make_pair(n,m);
        };

        void clear()
        {
            n=m=m_size=0;
            if(matrix!=nullptr)
            {
                delete []matrix;
                matrix=nullptr;
            }
        }

    public:
        const T &operator()(size_t i, size_t j) const//返回元素
        {
            if(i>=n||j>=m||i<0||j<0) throw std::invalid_argument("element failed");
            return matrix[i*m+j];
        }

        T &operator()(size_t i, size_t j)
        {
            if(i>=n||j>=m||i<0||j<0) throw std::invalid_argument("element failed");
            return matrix[i*m+j];
        }

        Matrix<T> row(size_t i) const
        {
            if(i>=n||i<0) throw std::invalid_argument("row failed");
            Matrix<T> tmp(1,m);
            for(size_t j=0;j<m;++j) tmp.matrix[j]=matrix[i*m+j];
            return tmp;
        }

        Matrix<T> column(size_t i) const
        {
            if(i>=m||i<0) throw std::invalid_argument("column failed");
            Matrix<T> tmp(n,1);
            for(size_t j=0;j<n;++j) tmp.matrix[j]=matrix[j*m+i];
            return tmp;
        }

    public:
        template <class U>
        bool operator==(const Matrix<U> &o) const
        {
            if(size()!=o.size()) return false;
            for(size_t i=0;i<m_size;++i)
                if(matrix[i]!=o(i/m,i%m)) return false;
            return true;
        }

        template <class U>
        bool operator!=(const Matrix<U> &o) const
        {
            if(size()!=o.size()) return true;
            for(size_t i=0;i<m_size;++i)
                if(matrix[i]!=o(i/m,i%m)) return true;
            return false;
        }

        Matrix operator-() const
        {
            for(size_t i=0;i<m_size;++i) matrix[i]=-matrix[i];
            return *this;
        }

        template <class U>
        Matrix &operator+=(const Matrix<U> &o)//强制类型转换
        {
            if(size()!=o.size()) throw std::invalid_argument("+= failed");
            for(size_t i=0;i<n;++i)
                for(size_t j=0;j<m;++j)
                    matrix[i*m+j]+=o(i,j);
            return *this;
        }

        template <class U>
        Matrix &operator-=(const Matrix<U> &o)
        {
            if(size()!=o.size()) throw std::invalid_argument("-= failed");
            for(size_t i=0;i<n;++i)
                for(size_t j=0;j<m;++j)
                    matrix[i*m+j]-=o(i,j);
            return *this;
        }

        template <class U>
        Matrix &operator*=(const U &x)
        {
            for(size_t i=0;i<m_size;++i) matrix[i]*=x;
            return *this;
        }

        Matrix tran() const
        {
            Matrix<T> transpose;
            transpose.n=m;
            transpose.m=n;
            transpose.m_size=m_size;
            transpose.matrix=new T[m_size];
            for(size_t i=0;i<m;++i)
                for(size_t j=0;j<n;++j)
                    transpose.matrix[i*n+j]=matrix[j*m+i];
            return transpose;
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

            friend iterator Matrix::begin();
            friend iterator Matrix::end();
            friend std::pair<iterator, iterator> Matrix::subMatrix(std::pair<size_t, size_t> l, std::pair<size_t, size_t> r);

        private:
            pointer p;
            size_type it_n,it_m,begin_n,begin_m,end_n,end_m;

        public:
            difference_type operator-(const iterator &o)
            {
                return p-o.p;
            }

            iterator &operator+=(difference_type offset)
            {
                p+=offset;
                return *this;
            }

            iterator operator+(difference_type offset) const
            {
                iterator it = *this;
                it += offset;
                return it;
            }

            iterator &operator-=(difference_type offset)
            {
                p-=offset;
                return *this;
            }

            iterator operator-(difference_type offset) const
            {
                iterator it = *this;
                it -= offset;
                return it;
            }

            iterator &operator++()//++i
            {
                (*this)+=1;
                return *this;
            }

            iterator operator++(int)//i++
            {
                iterator tmp=*this;
                (*this)+=1;
                return tmp;
            }

            iterator &operator--()
            {
                (*this)-=1;
                return *this;
            }

            iterator operator--(int)
            {
                iterator tmp=*this;
                (*this)-=1;
                return tmp;
            }

            reference operator*() const
            {
                return *p;
            }

            pointer operator->() const
            {
                return p;
            }

            bool operator==(const iterator &o) const
            {
                if(p==o.p) return true;
                else return false;
            }

            bool operator!=(const iterator &o) const
            {
                if(p!=o.p) return true;
                else return false;
            }
        };

        iterator begin()
        {
            iterator tmp;
            tmp.p=matrix;
            tmp.it_n=0;
            tmp.it_m=0;
            tmp.begin_n=0;
            tmp.begin_m=0;
            tmp.end_n=n;
            tmp.end_m=m;
            return tmp;
        }

        iterator end()
        {
            iterator tmp;
            tmp.p=matrix;
            tmp.it_n=n;
            tmp.it_m=0;
            tmp.begin_n=0;
            tmp.begin_m=0;
            tmp.end_n=n;
            tmp.end_m=m;
            return tmp;
        }

        std::pair<iterator, iterator> subMatrix(std::pair<size_t, size_t> l, std::pair<size_t, size_t> r)
        {
            iterator it_begin,it_end;

            it_begin.p=matrix;
            it_begin.it_n=0;
            it_begin.it_m=0;
            it_begin.begin_n=l.first;
            it_begin.begin_m=l.second;
            it_begin.end_n=r.first;
            it_begin.end_m=r.second;

            it_end.p=matrix;
            it_end.it_n=r.first-l.first;
            it_end.it_m=0;
            it_end.begin_n=l.first;
            it_end.begin_m=l.second;
            it_end.end_n=r.first;
            it_end.end_m=r.second;

            return std::make_pair(it_begin,it_end);
        }

    };

}

//
namespace sjtu
{
    template <class V, class U>
    auto operator*(const Matrix<V> &mat, const U &x)
    {
        Matrix<decltype(mat.matrix[0]*x)> tmp(mat.n,mat.m);
        for(size_t i=0;i<mat.m_size;++i) tmp.matrix[i]=mat.matrix[i]*x;
        return tmp;
    }

    template <class V, class U>
    auto operator*(const U &x, const Matrix<V> &mat)
    {
        Matrix<decltype(mat.matrix[0]*x)> tmp(mat.n,mat.m);
        for(size_t i=0;i<mat.m_size;++i) tmp.matrix[i]=mat.matrix[i]*x;
        return tmp;
    }

    template <class U, class V>
    auto operator*(const Matrix<U> &a, const Matrix<V> &b)
    {
        if(a.columnLength() != b.rowLength()) throw std::invalid_argument("* failed");
        Matrix<decltype(a(0,0)*b(0,0))> tmp(a.n,b.m);
        for(size_t i=0;i<tmp.m_size;++i) tmp.matrix[i]=0;
        for(size_t i=0;i<a.n;++i)
            for(size_t j=0;j<b.m;++j)
                for(size_t k=0;k<a.m;++k)
                    tmp.matrix[i*b.m+j]+=a.matrix[i*a.m+k]*b.matrix[k*b.m+j];
        return tmp;
    }

    template <class U, class V>
    auto operator+(const Matrix<U> &a, const Matrix<V> &b)//decltype
    {
        if(a.columnLength() != b.columnLength() || a.rowLength() != b.rowLength())
            throw std::invalid_argument("+ failed");
        Matrix<decltype(a(0,0)+b(0,0))> tmp(a.n,a.m);
        for(size_t i=0;i<tmp.m_size;++i) tmp.matrix[i]=a.matrix[i]+b.matrix[i];
        return tmp;
    }

    template <class U, class V>
    auto operator-(const Matrix<U> &a, const Matrix<V> &b)
    {
        if(a.columnLength() != b.columnLength() || a.rowLength() != b.rowLength())
            throw std::invalid_argument("- failed");
        Matrix<decltype(a(0,0)-b(0,0))> tmp(a.n,a.m);
        for(size_t i=0;i<tmp.m_size;++i) tmp.matrix[i]=a.matrix[i]-b.matrix[i];
        return tmp;
    }

}

#endif //SJTU_MATRIX_HPP

