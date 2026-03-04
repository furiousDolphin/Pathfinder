

#ifndef MATLAB_HPP_
#define MATLAB_HPP_

#include <vector>
#include <algorithm>
#include <initializer_list>
#include <cmath>
#include <stdexcept>
#include <functional>
#include <iostream>
#include <sstream>
#include <numeric>
#include <cstdlib>
#include <iomanip>


namespace MatLab
{

    template <typename T>
    bool is_close(T a, T b, double rtol = 1e-05, double atol = 1e-08) 
    { return std::abs(a - b) <= (atol + rtol * std::abs(b)); }


    template<typename T>
    class Vector
    {
        using Collection = std::vector<T>;

        public:
            Vector():
                v_{},
                stride_{1},
                view_ptr_{nullptr},
                size_{0},
                is_view_{false},
                is_transposed_{false}
            {}

            ~Vector() = default;

            Vector( const Vector<T>& other ):
                Vector{}
            {
                std::size_t n = other.size();
                v_.assign(n, T{});
                for ( std::size_t k = 0; k < n; k++ )
                { v_[k] = other[k]; }
            }

            Vector( Vector&& other ) = default;

            // Vector<T>& operator=( const Vector<T>& other )
            // {
            //     Vector<T> temp{other};
            //     std::swap(v_, temp.v_);
            //     std::swap(stride_, temp.stride_);
            //     std::swap(view_ptr_, temp.view_ptr_);
            //     std::swap(size_, temp.size_);
            //     std::swap(is_view_, temp.is_view_);
            //     std::swap(is_transposed_, temp.is_transposed_);
            //     return *this;
            // }

            Vector<T>& operator=( Vector<T>&& other ) = default;

            Vector(std::size_t n, T val = T{0} ):
                Vector{}
            { v_.assign( n, val ); }

            Vector(T* start_ptr, std::size_t n, std::size_t stride):
                Vector{}
            {
                view_ptr_ = start_ptr;
                size_ = n;
                stride_ = stride;
                is_view_ = true;
            }

            Vector( const Collection& v ):
                Vector{}
            { v_ = v; } 

            Vector( Collection&& v ):
                Vector{}
            { v_ = std::move(v); }

            Vector( std::initializer_list<T> init ):
                Vector{}
            { v_= init; }

            void push_back( T val )
            { v_.push_back(val); }

            void pop_back()
            { v_.pop_back(); }

            std::size_t size() const
            { return is_view_ ? size_ : v_.size(); }



            Vector<T>& operator=( const Vector<T>& other )
            {
                if ( size_ != other.size() )
                { throw std::runtime_error("dlugosci wektorow sa rozne"); }
                
                if (this == &other) return *this;

                for ( std::size_t i = 0; i < size_; i++ )
                { (*this)[i] = other[i]; }

                return *this; 
            }

            Vector<T>& operator+=( const Vector<T>& other )
            {
                if ( this->size() != other.size() )
                { throw std::runtime_error("dlugosci wektorow sa rozne"); }

                std::transform( this->begin(), this->end(), other.begin(), this->begin(), [](T elem1, T elem2){ return elem1+elem2; } );
                return *this;    
            }

            Vector<T>& operator+=( T scalar )
            { 
                std::transform( this->begin(), this->end(), this->begin(), [&scalar](T elem){ return elem+scalar; } );
                return *this;
            }          


            Vector<T>& operator-=( const Vector<T>& other )
            {
                if ( this->size() != other.size() )
                { throw std::runtime_error("dlugosci wektorow sa rozne"); }

                std::transform( this->begin(), this->end(), other.begin(), this->begin(), [](T elem1, T elem2){ return elem1-elem2; } );
                return *this;    
            }
            
            Vector<T>& operator-=( T scalar )
            { 
                std::transform( this->begin(), this->end(), this->begin(), [&scalar](T elem){ return elem-scalar; } );
                return *this;
            }          

            
            Vector<T>& operator*=( const Vector<T>& other )
            {
                if ( this->size() != other.size() )
                { throw std::runtime_error("dlugosci wektorow sa rozne"); }

                std::transform( this->begin(), this->end(), other.begin(), this->begin(), [](T elem1, T elem2){return elem1*elem2;} );
                return *this;
            }

            Vector<T>& operator*=( T scalar )
            {
                std::transform(this->begin(), this->end(), this->begin(), [&scalar](T elem){return elem*scalar;});
                return *this;
            }


            Vector<T>& operator/=( const Vector<T>& other )
            {
                if ( this->size() != other.size() )
                { throw std::runtime_error("dlugosci wektorow sa rozne"); }

                std::transform( 
                    this->begin(), this->end(), other.begin(), this->begin(), 
                    [](T elem1, T elem2)
                    {
                        if ( is_close(elem2, T{0} ) )
                        { throw std::runtime_error("dzielenie przez 0 w Vector<T>/= lub /"); }  
                        return elem1/elem2;
                    } 
                );

                return *this;
            }

            Vector<T>& operator/=( T scalar )
            {
                if ( is_close(scalar, T{0} ) )
                { throw std::runtime_error("dzielenie przez 0 w Vector<T>/= lub /"); }
                
                std::transform(this->begin(), this->end(), this->begin(), [&scalar](T elem){return elem/scalar;});
                return *this;
            }


            Vector<T> operator+( const Vector<T>& other ) const
            { 
                Vector<T> result{*this};
                result+=other;
                return result;
            }

            Vector<T> operator+( T scalar ) const
            { 
                Vector<T> result{*this};
                result+=scalar;
                return result;
            }

            friend Vector<T> operator+( T scalar, const Vector<T>& v )
            {
                return v+scalar;
            }


            Vector<T> operator-( const Vector<T>& other ) const
            { 
                Vector<T> result{*this};
                result-=other;
                return result;
            }

            Vector<T> operator-( T scalar ) const 
            { 
                Vector<T> result{*this};
                result-=scalar;
                return result; 
            }            

            friend Vector<T> operator-( T scalar, const Vector<T>& v )
            { return v-scalar; }


            Vector<T> operator*( const Vector<T>& other ) const
            { 
                Vector<T> result{*this};
                result*=other;
                return result;
            }

            Vector<T> operator*( T scalar ) const 
            { 
                Vector<T> result{*this};
                result*=scalar;
                return result; 
            }            

            friend Vector<T> operator*( T scalar, const Vector<T>& v )
            { return v*scalar; }


            Vector<T> operator/( const Vector<T>& other ) const
            { 
                Vector<T> result{*this};
                result/=other;
                return result;
            }

            Vector<T> operator/( T scalar ) const 
            { 
                Vector<T> result{*this};
                result/=scalar;
                return result; 
            }            

            friend Vector<T> operator/( T scalar, const Vector<T>& v )
            { return v/scalar; }


            T dot( const Vector<T>& other ) const
            { return std::inner_product( this->begin(), this->end(), other.begin(), T{0} ); }  

            double norm() const
            {   
                auto res = std::accumulate( this->begin(), this->end(), T{0}, [](T acc, T elem){ return acc + elem*elem; } );
                return std::sqrt( static_cast<double>(res) );
            }

            Vector<T>& normilize()
            {
                double length = this->norm();

                if ( is_close(length, T{0} ) )
                { throw std::runtime_error("norma wektora bliska zeru"); }

                return *this/=length;
            }

            Vector<T> normilized() const
            { 
                Vector<T> result{*this};
                result.normilize();
                return result;    
            }

            Vector<T>& transpose()
            { 
                is_transposed_ =! is_transposed_;
                return *this; 
            }

            Vector<T> transposed() const
            {
                Vector<T> result{*this};
                result.transpose();
                return result;
            }

            bool any_is_close(T val = T{0}, double rtol = 1e-05, double atol = 1e-08) const
            {
                for ( auto it = this->begin(); it != this->end(); it++ )
                { 
                    if ( is_close(*it, val, rtol, atol) )
                    { return true; }
                }
                return false;
            }

            Vector<T> slice( std::size_t offset, std::size_t count )
            {
                if ( offset+count > this->size() )
                { throw std::runtime_error("wycinek wychodzi za end"); }

                T* new_start = is_view_ ? view_ptr_ : const_cast<T*>(v_.data()) + ( offset*stride_ );

                return Vector<T>{ new_start, count, stride_ };
            }

            std::string to_string() const
            {   
                if (size_ == 0) return "()";

                std::ostringstream oss;
                oss << "( " << std::fixed << std::setprecision(3);

                auto it = this->begin();
                auto end = this->end();
                for ( ; it != end; it++ )
                { 
                    auto next = it;
                    next++;

                    oss << std::setw(7) << *it;
                    oss << ( next != end ? "; " : ")" ); 
                }
                return oss.str();
            }

            friend std::ostream& operator<<(std::ostream& os, const Vector<T>& v) 
            {
                os << v.to_string();
                return os;
            }

            template <typename Func>
            Vector<T> apply(Func func) const
            {
                Vector<T> result;
                result.v_.reserve(this->size());

                for ( auto it = this->begin(); it != this->end(); it++ )
                {result.v_.push_back(func(*it));}

                return result;
            }


            Vector<T> operator-() const
            {
                Vector<T> result{*this};
                std::transform(this->begin(), this->end(), result.begin(), [](T elem){return -elem;});
                return result;
            }


            const T& operator[](std::size_t idx) const 
            { return is_view_ ? view_ptr_[idx*stride_] : v_[idx]; }

            T& operator[](std::size_t idx) 
            { return const_cast<T&>( static_cast<const Vector<T>&>(*this)[idx] ); }

            struct StrideIterator
            {
                T* ptr;
                std::size_t stride;

                T& operator*() const { return *ptr; }
                StrideIterator& operator++(int) { ptr += stride; return *this; }
                StrideIterator& operator++() { ptr += stride; return *this; }
                bool operator!=(const StrideIterator& other) const { return ptr != other.ptr; }
            };

            StrideIterator begin() const
            {
                T* start = is_view_ ? view_ptr_ : const_cast<T*>(v_.data());
                return StrideIterator{start, stride_};
            }

            StrideIterator end() const
            {
                T* start = is_view_ ? view_ptr_ : const_cast<T*>(v_.data());
                return StrideIterator{start+this->size()*stride_, stride_};
            }




        private:
            Collection v_;
            std::size_t stride_;
            T* view_ptr_;
            std::size_t size_;
            bool is_view_;
            bool is_transposed_;
            
    };


    template<typename T>
    class Matrix
    {
        using Collection = std::vector<T>;

        public:

            Matrix() = default;

            Matrix( std::size_t rows, std::size_t cols ):
                data_(rows*cols, T{0}),
                rows_{rows},
                cols_{cols},
                row_stride_{cols},
                col_stride_{1},
                is_view_{false},
                view_ptr_{nullptr}
            {}

            Matrix(std::initializer_list<std::initializer_list<T>> init)
            {
                std::size_t rows = init.size();
                std::size_t cols = 0;

                for ( const auto& row : init )
                { cols = std::max(row.size(), cols); }

                data_.assign(rows*cols, T{0});
                rows_ = rows;
                cols_ = cols;
                row_stride_ = cols;
                col_stride_ = 1;
                is_view_ = false;
                view_ptr_ = nullptr;

                std::size_t i = 0;
                for ( const auto& row : init )
                {
                    std::size_t j = 0;
                    for ( const auto& val : row )
                    { 
                        (*this)(i, j) = val;
                        j++;
                    }   
                    i++;
                }
            }

            Matrix(const Matrix& other):
                Matrix{other.rows_, other.cols_}
            {
                for ( std::size_t i = 0; i < other.rows_; i++ )
                {
                    for ( std::size_t j = 0; j < other.cols_; j++ )
                    {
                        (*this)(i, j) = other(i, j);
                    }
                }
            }

            Matrix(Matrix&& other) = default;

            Matrix(T* view_ptr, std::size_t rows, std::size_t cols, std::size_t row_stride, std::size_t col_stride ):
                data_{},
                is_view_{true},
                view_ptr_{view_ptr},
                rows_{rows},
                cols_{cols},
                row_stride_{row_stride},
                col_stride_{col_stride}
            {}


            Matrix<T>& operator=( const Matrix<T>& other )
            {
                if ( rows_ != other.rows_ || cols_ != other.cols_ )
                { throw std::runtime_error("inne wymiary macierzy w operatorze ="); }

                if ( this == &other )
                { return *this; }

                if ( is_view_ )
                {
                    for ( std::size_t i = 0; i < rows_; i++ )
                    {
                        for ( std::size_t j = 0; j < cols_; j++ )
                        { (*this)(i, j) = other(i, j); }
                    }
                }

                else
                {
                    Matrix<T> temp{other};
                    std::swap(data_, temp.data_);
                    std::swap(row_stride_, temp.row_stride_);
                    std::swap(col_stride_, temp.col_stride_);
                    std::swap(view_ptr_, temp.view_ptr_);
                    std::swap(rows_, temp.rows_);
                    std::swap(cols_, temp.cols_);
                    std::swap(is_view_, temp.is_view_);
                }
                return *this;
            }

            Matrix<T>& operator=( Matrix<T>&& other )
            {
                if (this == &other) return *this;
                    
                data_ = std::move(other.data_);
                rows_ = other.rows_;
                cols_ = other.cols_;
                row_stride_ = other.row_stride_;
                col_stride_ = other.col_stride_;
                view_ptr_ = other.view_ptr_;
                is_view_ = other.is_view_;
                
                other.view_ptr_ = nullptr;
                other.rows_ = 0;
                other.cols_ = 0;
                return *this;
            }            
            
            const T& operator()( std::size_t row, std::size_t col ) const
            { 
                std::size_t idx = row*row_stride_ + col*col_stride_;
                return is_view_ ? view_ptr_[idx] : data_[idx];                  
            } 

            T& operator()( std::size_t row, std::size_t col )
            { return const_cast<T&>( (static_cast< const Matrix<T>& >(*this))(row, col) ); }

            Matrix<T>& transpose()
            {
                std::swap(rows_, cols_);
                std::swap(row_stride_, col_stride_);
                return *this;
            }
            
            Vector<T> get_row(std::size_t row) const 
            {
                T* base = is_view_ ? view_ptr_ : const_cast<T*>(data_.data());
                T* start = base + (row * row_stride_);
                return Vector<T>{ start, cols_, col_stride_ };
            }

            Vector<T> get_col(std::size_t col) const 
            {
                T* base = is_view_ ? view_ptr_ : const_cast<T*>(data_.data());
                T* start = base + (col * col_stride_);
                return Vector<T>{ start, rows_, row_stride_ };
            }


            Matrix<T> slice(std::size_t start_row, std::size_t rows, std::size_t start_col, std::size_t cols ) const
            {
                if ( start_row+rows > rows_ || start_col+cols > cols_ )
                { throw std::runtime_error("za duzy wycinek macierzy"); }

                T* ptr = ( is_view_ ? view_ptr_ : const_cast<T*>(data_.data()) ) + ( start_row*row_stride_ + start_col*col_stride_ );

                return Matrix<T>{ ptr, rows, cols, row_stride_, col_stride_ };
            }

            std::array<std::size_t, 2> size() const 
            { return {rows_, cols_}; }


            struct StrideIterator
            {
                T* ptr;
                std::size_t n;
                std::size_t vector_stride;
                std::size_t increment_stride;

                Vector<T> operator*() const
                { return Vector<T>{ ptr, n, vector_stride }; }

                StrideIterator& operator++(int)
                { 
                    ptr+=increment_stride;
                    return *this;
                }

                StrideIterator& operator++()
                { 
                    ptr+=increment_stride;
                    return *this;
                }

                bool operator!=(const StrideIterator& other) const
                { return ptr!=other.ptr; }
            };


            Matrix<T>& operator+=( const Matrix<T>& other )
            {
                if ( rows_ != other.rows_ || cols_ != other.cols_ )
                { throw std::runtime_error("nie mozna dodac macierzy o roznych wymiarach"); }

                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) += other(i, j); } 
                }
                
                return *this;
            }

            Matrix<T> operator+=( T scalar )
            {
                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) += scalar; } 
                }
                return *this;
            }

            Matrix<T> operator-=( const Matrix<T>& other )
            {
                if ( rows_ != other.rows_ || cols_ != other.cols_ )
                { throw std::runtime_error("nie mozna dodac macierzy o roznych wymiarach"); }

                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) -= other(i, j); } 
                }
                
                return *this;
            }

            Matrix<T> operator-=( T scalar )
            {
                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) -= scalar; } 
                }
                return *this;
            }

            Matrix<T> operator*=( const Matrix<T>& other )
            {
                if ( rows_ != other.rows_ || cols_ != other.cols_ )
                { throw std::runtime_error("nie mozna dodac macierzy o roznych wymiarach"); }

                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) *= other(i, j); } 
                }
                
                return *this;
            }

            Matrix<T> operator*=( T scalar )
            {
                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) *= scalar; } 
                }
                return *this;
            }

            Matrix<T> operator/=( const Matrix<T>& other )
            {
                if ( rows_ != other.rows_ || cols_ != other.cols_ )
                { throw std::runtime_error("nie mozna dodac macierzy o roznych wymiarach"); }

                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    {
                        T denominator = other(i, j);
                        if ( is_close(denominator, T{0}) )
                        { throw std::runtime_error("nie mozna dodac macierzy o roznych wymiarach"); } 
                        (*this)(i, j) /= denominator; 
                    } 
                }
                
                return *this;
            }

            Matrix<T> operator/=( T scalar )
            {
                if ( is_close(scalar, T{0}) )
                { throw std::runtime_error("nie mozna dodac macierzy o roznych wymiarach"); }

                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    for ( std::size_t j = 0; j < cols_; j++ )
                    { (*this)(i, j) /= scalar; } 
                }
                return *this;
            }


            Matrix<T> operator+( const Matrix<T>& other ) const
            { 
                Matrix<T> result{*this};
                result+=other;
                return result;
            }

            Matrix<T> operator+( T scalar ) const
            { 
                Matrix<T> result{*this};
                result+=scalar;
                return result;
            }

            friend Matrix<T> operator+( T scalar, const Matrix<T>& v )
            {
                return v+scalar;
            }


            Matrix<T> operator-( const Matrix<T>& other ) const
            { 
                Matrix<T> result{*this};
                result-=other;
                return result;
            }

            Matrix<T> operator-( T scalar ) const 
            { 
                Matrix<T> result{*this};
                result-=scalar;
                return result; 
            }            

            friend Matrix<T> operator-( T scalar, const Matrix<T>& v )
            { return v-scalar; }


            Matrix<T> operator*( const Matrix<T>& other ) const
            { 
                Matrix<T> result{*this};
                result*=other;
                return result;
            }

            Matrix<T> operator*( T scalar ) const 
            { 
                Matrix<T> result{*this};
                result*=scalar;
                return result; 
            }            

            friend Matrix<T> operator*( T scalar, const Matrix<T>& v )
            { return v*scalar; }


            Matrix<T> operator/( const Matrix<T>& other ) const
            { 
                Matrix<T> result{*this};
                result/=other;
                return result;
            }

            Matrix<T> operator/( T scalar ) const 
            { 
                Matrix<T> result{*this};
                result/=scalar;
                return result; 
            }            

            friend Matrix<T> operator/( T scalar, const Matrix<T>& v )
            { return v/scalar; }
          

            Matrix<T> dot(const Matrix<T>& other) const
            {
                if ( cols_ != other.rows_ )
                { throw std::runtime_error("nie mozna mnozyc macierzy o roznych wymiarach"); }

                Matrix<T> result{rows_, other.cols_};

                for ( std::size_t i = 0; i < rows_; i++ )
                {
                    auto row = this->get_row(i);
                    for ( std::size_t j = 0; j < other.cols_; j++ )
                    { 
                        result(i, j) = row.dot(other.get_col(j));
                    } 
                }
                return result;
            }


            StrideIterator col_begin() const
            {
                T* start = is_view_ ? view_ptr_ : const_cast<T*>(data_.data());
                return StrideIterator{ start, rows_, row_stride_, col_stride_ };
            }

            StrideIterator col_end() const
            {
                T* start = is_view_ ? view_ptr_ : const_cast<T*>(data_.data());
                return StrideIterator{ start+cols_*col_stride_, rows_, row_stride_, col_stride_  };
            }

            StrideIterator row_begin() const
            {
                T* start = is_view_ ? view_ptr_ : const_cast<T*>(data_.data());
                return StrideIterator{ start, cols_, col_stride_, row_stride_ };
            }

            StrideIterator row_end() const
            {
                T* start = is_view_ ? view_ptr_ : const_cast<T*>(data_.data());
                return StrideIterator{ start+rows_*row_stride_, cols_, col_stride_, row_stride_  };
            }

            struct Range
            {
                StrideIterator b, e;
                StrideIterator begin() const { return b; }
                StrideIterator end() const { return e; }
            };

            Range rows() const 
            { return Range{ this->row_begin(), this->row_end() }; }

            Range cols() const 
            { return Range{ this->col_begin(), this->col_end() }; }

            std::string to_string() const
            {
                std::ostringstream oss;
                
                for ( const auto& row : this->rows() )
                { 
                    oss << row << std::endl;
                }
                return oss.str();
            }

            friend std::ostream& operator<<( std::ostream& os, const Matrix<T>& m )
            { return os << m.to_string(); }
            
            Matrix<T> operator-() const
            {
                Matrix<T> result{*this}; 
                for (std::size_t i = 0; i < rows_; i++)
                {
                    for (std::size_t j = 0; j < cols_; j++)
                    { result(i, j) = -result(i, j); }
                }
                return result;
            }

            template<typename Func>
            Matrix<T> apply(Func func) const
            {
                Matrix<T> result{rows_, cols_};

                if ( !is_view_ )
                { std::transform(data_.begin(), data_.end(), result.data_.begin(), func ); }
                else 
                { std::transform(this->row_begin(), this->row_end(), result.row_begin(), [&](auto row_view) {return row_view.apply(func); } ); }

                return result;
            }

            template<typename Func>
            Matrix<T>& apply_inplace(Func func) 
            {
                for (std::size_t i = 0; i < rows_; ++i) 
                {
                    for (std::size_t j = 0; j < cols_; ++j) 
                    {
                        T& val = (*this)(i, j);
                        val = func(val);
                    }
                }
                return *this;
            }


        private:
            Collection data_;
            std::size_t rows_, cols_;
            std::size_t row_stride_;
            std::size_t col_stride_;
            bool is_view_;
            T* view_ptr_;
    };


    template<typename T>
    std::pair<Matrix<T>, Matrix<T>> meshgrid(std::size_t n, std::size_t m, T start, T stop)
    {

        MatLab::Matrix<T> X{n, m};
        MatLab::Matrix<T> Y{n, m};

        T dx = (stop - start) / (m - 1);
        T dy = (stop - start) / (n - 1);

        MatLab::Vector<T> x_coords(m);
        for(std::size_t j = 0; j < m; ++j) 
        { x_coords[j] = start + j * dx; }


        MatLab::Vector<T> y_coords(n);
        for(std::size_t i = 0; i < n; ++i) 
        { y_coords[i] = start + i * dy; }


        for (std::size_t i = 0; i < n; i++) 
        { X.get_row(i) = x_coords; }

        for (std::size_t j = 0; j < m; j++) 
        { Y.get_col(j) = y_coords; }

        return std::pair{ X, Y };
    }
                    

}

#endif