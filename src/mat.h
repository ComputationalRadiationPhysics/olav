/**
* Copyright 2015 Alexander Matthes
*
* This file is part of OLAV.
*
* OLAV is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OLAV is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OLAV.
* If not, see <http://www.gnu.org/licenses/>.
*/

#pragma	once
#include "vec.h"
#include <limits> 
#include <cassert>


/**
* A matrix type (full column major storage) 
* The matrix can be loaded directly into OpenGL without need for transposing!
*/


template <typename T>
class mat 
{
private:
	///pointer to data storage
	vec<T> _data;
	///number of columns
	unsigned _ncols;
	///number of rows
	unsigned _nrows;


public:
	///standard constructor 
	mat()
	{
		_ncols = 0;
		_nrows = 0;
	}


	///constructor creates a nrows x ncols full matrix
	mat(unsigned nrows, unsigned ncols) : _data(nrows*ncols)
	{
		_nrows = nrows;
		_ncols = ncols;			
	}
	
	///construct a matrix with all elements set to c
	mat(unsigned nrows, unsigned ncols, const T& c) :_data(nrows*ncols)
	{
		_nrows = nrows;
		_ncols = ncols;				
		_data.fill(c);
	}


	///creates a matrix from an array 
	///if the matrix data is stored in a row major fashion set column_major to false
	mat(unsigned nrows, unsigned ncols, const T* marray,bool column_major=true) 
		:_data(nrows*ncols)
	{ 
		_nrows = nrows;
		_ncols = ncols;	
		if(column_major)
			memcpy(_data, marray, size()*sizeof(T));
		else
		{
			
			for(unsigned i = 0; i < _nrows; i++)
				for(unsigned j = 0; j < _ncols;j++)
				{
					operator()(i,j) = marray[i*_ncols +j];
				}
				
			
		}
	}



	///copy constructor for matrix with different element type
	template <typename S>
	mat(const mat<S>& m):_data(m.size())
	{ 
		_nrows = m.nrows();
		_ncols = m.ncols();	
		for(unsigned i = 0; i < _nrows; i++)
			for(unsigned j = 0; j < _ncols;j++)
			{
				operator()(i,j)=(T)m(i,j);
			}

	}
	
	
	///number of stored elements
	unsigned size() const 
	{
		return _data.size();
	}

	///number of rows
	unsigned nrows() const
	{
		return _nrows;
	}

	///number of columns
	unsigned ncols() const
	{
		return _ncols;
	}

	
	
	///assignment of a matrix with a different element type
	template <typename S> 
	mat<T>& operator = (const mat<S>& m) 
	{  
		resize(m.nrows(),m.ncols());
		for(unsigned i = 0; i < _nrows; i++)
			for(unsigned j = 0; j < _ncols;j++)
			{
				operator()(i,j)=(T)m(i,j);
			}
		return *this;
	}

	///assignment of a scalar s to each element of the matrix
	mat<T>& operator  = (const T& s) 
	{ 
		fill (s);
		return *this; 
	}

	///resize the matrix, the content of the matrix will be destroyed
	void resize(unsigned rows, unsigned cols)
	{
		
		unsigned newsize = rows*cols;
		_nrows = rows;
		_ncols = cols;
		_data.resize(newsize);
	}

	///cast operator for non const array 
	operator T*()
	{
		return (T*)_data;
	}

	///cast operator const array
	operator const T*() const
	{
		return (const T*)_data;
	}

	///returns true if matrix is a square matrix
	bool is_square() const
	{
		return _ncols == _nrows;
	}

	///fills all elements of the matrix with v
	template <typename S>
	void fill(const S& v)
	{
		T val = (T)v;
		_data.fill(val); 
	}

	///access to the element in the ith row in column j
	T& operator () (unsigned i, unsigned j)  
	{
		assert(i < _nrows && j < _ncols);
		return _data[j*_nrows+i]; 
	}
	
	///const access to the element in the ith row on column j 
	const T operator () (unsigned i, unsigned j) const 
	{
		assert(_data != NULL && i < _nrows && j < _ncols);
		return _data[j*_nrows+i]; 
	}

	///test for equality
	template <typename S> 
	bool operator == (const mat<S>& m) const
	{ 
		if(_ncols != m.ncols() || _nrows != m.nrows())
			return false;

		return _data == m._data;
	}

	///test for inequality
	template <typename S> 
	bool operator != (const mat<S>& m) const
	{ 
		if(_ncols != m.ncols() || _nrows != m.nrows())
			return false;
		return _data != m._data;
		return false; 
	}


	//in place scalar multiplication
	mat<T>& operator *= (const T& s)			
	{ 
		_data *= (T)s;
		return *this; 
	}

	///scalar multiplication  
	const mat<T> operator*(const T& s) const
	{		
		mat<T> r=(*this);
		r*=(T)s;
		return r;
	}

	///in place division by a scalar
	mat<T>& operator /= (const T& s)			
	{ 
		_data /= (T)s;
		return *this; 
	}

	/// division by a scalar
	const mat<T> operator / (const T& s)			
	{ 
		mat<T> r=(*this);
		r/=s;
		return r;
	}


	///in place addition by a scalar
	mat<T>& operator += (const T& s)			
	{ 
		_data += (T)s;
		return *this; 
	}

	///componentwise addition of a scalar
	const mat<T> operator + (const T& s)			
	{ 
		mat<T> r=(*this);
		r+=s;
		return r;
	}

	///in place substraction of a scalar
	mat<T>& operator -= (const T& s)			
	{ 
		_data -= (T)s;
		return *this; 
	}

	/// componentwise subtraction of a scalar
	const mat<T> operator - (const T& s)			
	{ 
		mat<T> r=(*this);
		r-=s;
		return r;
	}

	///negation operator
	const mat<T> operator-() const
	{ 
		mat<T> r=(*this)*((T)-1);
		return r;
	}

	///in place addition of matrix
	template <typename S> 
	mat<T>& operator += (const mat<S>& m) 
	{ 
		assert(_ncols == m.ncols() &&  _nrows == m.nrows());
		_data+=m._data;
		return *this; 
	}

	///in place subtraction of matrix
	template <typename S> 
	mat<T>& operator -= (const mat<S>& m) 
	{ 
		assert(_ncols == m.ncols() &&  _nrows == m.nrows());
		_data-=m._data;
		return *this; 
	}

	

	
	///matrix addition
	template <typename S>
	const mat<T> operator+(const mat<S> m2)const
	{
		mat<T> r=(*this);
		r += m2; 
		return r;
	}

	///matrix subtraction
	template <typename S>
	const mat<T> operator-(const mat<S> m2)const
	{
		mat<T> r=(*this);
		r-= m2;
		return r;
	}


	///in place matrix multiplication with  a ncols x ncols matrix m2
	template <typename S>
	const mat<T> operator*=(const mat<S>& m2) 
	{
		assert(ncols() == m2.ncols() && nrows() == m2.nrows() && ncols() == nrows());
		mat<T> r(_nrows,_ncols,(T)0);
	
		for(unsigned i = 0; i < _nrows; i++)
			for(unsigned j = 0; j < _ncols;j++)
				for(unsigned k = 0; k < _ncols; k++)
					r(i,j) += operator()(i,k) * (T)(m2(k,j)); 
		(*this)=r;
	
		return *this;
	}

	

	///multiplication with a ncols x M matrix m2
	template <typename S>
	const mat<T> operator*(const mat<S>& m2) const
	{
		assert(m2.nrows() == _ncols);
		unsigned M = m2.ncols();
		mat<T> r(_nrows,M,(T)0);
		for(unsigned i = 0; i < _nrows; i++)
			for(unsigned j = 0; j < M;j++)
				for(unsigned k = 0; k < _ncols; k++)
					r(i,j) += operator()(i,k) * (T)(m2(k,j)); 
	
		return r;
	}


	///matrix vector multiplication
	template < typename S>
	const vec<T> operator*(const vec<S>& v) const
	{
		assert(_ncols==v.size());		
		vec<T> r;
		r.zeros(_nrows);
	
		for(unsigned i = 0; i < _nrows; i++)
			for(unsigned j = 0; j < _ncols; j++)
				r(i) += operator()(i,j) * (T)(v(j)); 
	
		return r;
	}

	///create submatrix m(top,left)...m(top+rows,left+cols)
	mat<T> sub_mat(unsigned top, unsigned left, unsigned rows, unsigned cols) const
	{
		
		mat<T> mnew(rows,cols);
	
		for(unsigned i = 0; i < rows;i++)
			for(unsigned j = 0; j < cols;j++) 
				mnew(i,j)=operator()(i+top,j+left);		
		
		return mnew;
	}

	///extract a row from the matrix as a vector
	const vec<T> row(unsigned i) const
	{
		
		vec<T> mnew(_ncols);
	
		
		for(unsigned j = 0; j < _ncols;j++) 
			mnew(j)=operator()(i,j);		
		
		return mnew;
	}

	///set row i of the matrix to vector v
	void set_row(unsigned i,const vec<T>& v) 
	{
			
		for(unsigned j = 0; j < _ncols;j++) 
			operator()(i,j)=v(j);		
		
	
	}

	///extract a column of the matrix as a vector
	const vec<T> col(unsigned j) const
	{
		
		vec<T> mnew(_nrows);
	
		
		for(unsigned i = 0; i < _nrows;i++) 
			mnew(i)=operator()(i,j);		
		
		return mnew;
	}

	///set  column j of the matrix to vector v
	void set_col(unsigned j,const vec<T>& v) 
	{
			
		for(unsigned i = 0; i < _nrows;i++) 
			operator()(i,j)=v(i);		
	}




	///copy submatrix m(top,left)...m(top+rows,left+cols) into submat
	void copy(unsigned top, unsigned left, unsigned rows, unsigned cols, mat<T>& submat) const
	{
		assert(submat.nrows() == rows && submat.ncols() == cols); 
			
		for(unsigned i = 0; i < rows;i++)
			for(unsigned j = 0; j < cols;j++) 
				submat(i,j)=operator()(i+top,j+left);		
		
		
	}
	
	///paste matrix m at position: top, left
	void paste(int top, int left,const mat<T>& m)
	{
		for(unsigned i = 0; i < m.nrows(); i++)
			for(unsigned j = 0; j < m.ncols(); j++) 
				operator()(i+top,j+left)=m(i,j);

	}


	///exchange row i with row j
	void swap_rows(unsigned i, unsigned j)
	{
		assert(i < _nrows && j < _nrows);		
		for(unsigned k = 0; k < _ncols;k++)
			std::swap(operator()(i,k),operator()(j,k));
	}
	
	///exchange column i with column j
	void swap_columns(unsigned i, unsigned j)
	{
		assert(i < _ncols && j < _ncols);		
		for(unsigned k = 0; k < _nrows;k++)
			std::swap(operator()(k,i),operator()(k,j));

	}

	///exchange diagonal elements (i,i) (j,j)
	void swap_diagonal_elements(unsigned i, unsigned j)
	{
		assert(i < _ncols && j < _ncols);		
		std::swap(operator()(i,i),operator()(j,j));
	}




	///returns the trace 
	T trace() const
	{
		assert(_nrows == _ncols);
		T t = 0;
		for(unsigned i = 0; i < _nrows;i++)
			t+=operator()(i,i);
		return t;
	}

	///transpose matrix
	void transpose()
	{
		mat<T> r(_ncols,_nrows);
	
		for(unsigned i = 0; i < _nrows;i++)
			for(unsigned j = 0; j < _ncols;j++)
				r(j,i) = operator()(i,j);

		*this = r;
	}

	//flip columns left-right
	void fliplr()
	{
		mat<T> r(_nrows,_ncols);
	
		for(unsigned i = 0; i < _nrows;i++)
			for(unsigned j = 0; j < _ncols;j++)
				r(i,j) = operator()(i,_ncols-j-1);

		*this = r;

	}

	//flip rows up-down
	void flipud()
	{
		mat<T> r(_nrows,_ncols);
	
		for(unsigned i = 0; i < _nrows;i++)
			for(unsigned j = 0; j < _ncols;j++)
				r(i,j) = operator()(_nrows-i-1,j);

		*this = r;

	}

	///ceil all components of the matrix
	void ceil()
	{
		for(unsigned i = 0; i < _nrows;i++)
			for(unsigned j = 0; j < _ncols;j++)
				operator()(i,j) =::ceil(operator()(i,j));
	}

	///floor all components of the matrix
	void floor()
	{
		for(unsigned i = 0; i < _nrows;i++)
			for(unsigned j = 0; j < _ncols;j++)
				operator()(i,j) =::floor(operator()(i,j));
	}
	
	///round to integer 
	void round()
	{
		for(unsigned i = 0; i < _nrows;i++)
			for(unsigned j = 0; j < _ncols;j++)
				operator()(i,j) =::floor(operator()(i,j)+(T)0.5);

	}
	

	
	///returns the frobenius norm of matrix m
	T frobenius_norm() const
	{
		T n=0;
		for(unsigned i =0; i < _nrows;i++)
			for(unsigned j=0;j <_ncols;j++)
				n+=operator()(i,j)*operator()(i,j);
	
		return (T)sqrt((double)n);
	}


	///set identity matrix
	void identity()
	{
		assert(_ncols == _nrows);
		zeros();
		for(unsigned i = 0; i < _ncols;++i)
			operator()(i,i)=1;
	}
	
	///set dim x dim identity matrix 
	void identity(unsigned dim)
	{
		zeros(dim,dim);
		for(unsigned i = 0; i < _ncols;++i)
			operator()(i,i)=1;
	}

	///set zero matrix
	void zeros()
	{
		fill(0);
	}

	///resize and fill matrix with zeros
	void zeros(unsigned rows, unsigned cols)
	{
		resize(rows,cols);
		fill((T)0);
	}

	///resize and fill matrix with ones
	void ones(unsigned rows, unsigned cols)
	{
		resize(rows,cols);
		fill((T)1);
	}




};

template <typename T>
mat<T> zeros(unsigned rows, unsigned cols)
{
	mat<T> m;
	m.zeros(rows,cols);
	return m;
}

template <typename T>
mat<T> ones(unsigned rows, unsigned cols)
{
	mat<T> m(rows,cols);
	m.fill((T)1);
	return m;
}

template<typename T>
mat<T> identity(unsigned dim)
{
	mat<T> m;
	m.identity(dim);
	return m;
}

//return frobenius norm
template<typename T>
T frobenius_norm(const mat<T>& m)
{
	return m.frobenius_norm();
}

//return trace of matrix
template<typename T>
T trace(const mat<T>& m)
{
	return m.trace();
}


//concatenate two matrices horizontally
template<typename T, typename S>
const mat<T> horzcat(const mat<T>& m1, const mat<S>& m2)
{
	
	assert(m1.nrows() == m2.nrows());
	mat<T> r(m1.nrows(),m1.ncols()+m2.ncols());
	for(unsigned i = 0; i < m1.nrows();i++)
	{
		for(unsigned j = 0; j < m1.ncols(); j++)
			r(i,j) = m1(i,j);
		for(unsigned j = 0; j < m2.ncols(); j++)
			r(i,j+m1.ncols())=(T)m2(i,j);
	}
	return r;
}

//concatenates a matrix and a column vector horizontally
template<typename T, typename S>
const mat<T> horzcat(const mat<T>& m1, const vec<S>& v)
{
	
	assert(m1.nrows() == v.size());
	mat<T> r(m1.nrows(),m1.ncols()+1);
	for(unsigned i = 0; i < m1.nrows();i++)
	{
		for(unsigned j = 0; j < m1.ncols(); j++)
			r(i,j) = m1(i,j);
		
		r(i,m1.ncols())=(T)v(i);
	}
	return r;
}

//concatenates two vectors horizontally
template<typename T, typename S>
const mat<T> horzcat(const vec<T>& v1, const vec<S>& v2)
{
	
	assert(v1.size() == v2.size());
	mat<T> r(v1.size(),2);
	for(unsigned i = 0; i < v1.size();i++)
	{
		r(i,0) = v1(i);
		r(i,1) = v2(i);
	}
	return r;
}

//concatenates two column vectors vertically
template<typename T, typename S>
const vec<T> vertcat(const vec<T>& v1, const vec<S>& v2)
{
	
	vec<T> r(v1.size()+v2.size());
	unsigned off = v1.size();
	for(unsigned i = 0; i < v1.size();i++)
		r(i) = v1(i);
	for(unsigned i = 0; i < v2.size();i++)
		r(i+off) = v2(i);
	
	return r;
}


//concatenates two matrices vertically
template<typename T, typename S>
const mat<T> vertcat(const mat<T>& m1, const mat<S>& m2)
{
	
	assert(m1.ncols() == m2.ncols());
	mat<T> r(m1.nrows()+m2.nrows(),m1.ncols());
	for(unsigned i = 0; i < m1.nrows();i++)
	{
		for(unsigned j = 0; j < m1.ncols(); j++)
			r(i,j) = m1(i,j);
	}

	for(unsigned i = 0; i < m2.nrows();i++)
	{
		for(unsigned j = 0; j < m1.ncols(); j++)
			r(i+m1.nrows(),j) = m2(i,j);
	}

	return r;
}


//transpose of a matrix m
template <typename T>
const mat<T> transpose(const mat<T> &m)
{
	mat<T> r = m;
	r.transpose();
	return r;
}

///ceil all components of the matrix
template <typename T>
const mat<T> ceil(const mat<T> &m)
{
	mat<T> r = m;
	r.ceil();
	return r;	
}

///floor all components of the matrix
template <typename T>
const mat<T> floor(const mat<T> &m)
{
	mat<T> r = m;
	r.floor();
	return r;	
}
	
///round all components of the matrix
template <typename T>
const mat<T> round(const mat<T> &m)
{
	mat<T> r = m;
	r.round();
	return r;	
}




///return the product of a scalar s and a matrix m
template <typename T> 
mat<T>  operator * (const T& s, const mat<T>& m)
{ 
	return m*(T)s; 
}



///output of a matrix onto an ostream
template <typename T>
std::ostream& operator<<(std::ostream& out, const mat<T>& m)
{
	
	for (unsigned i=0;i<m.nrows();++i)
	{
		for(unsigned j =0;j < m.ncols()-1;++j)
			out << m(i,j)<<" ";
		out << m(i,m.ncols()-1);
		if(i != m.nrows()-1)
			out <<"\n";
	}

	return out;

}


///input of a matrix onto an ostream
template <typename T>
std::istream& operator>>(std::istream& in, mat<T>& m)
{
	assert(m.size() > 0);
	for (unsigned i=0;i<m.nrows();++i)
		for(unsigned j =0;j < m.ncols();++j)
			in >> m(i,j);
	

	return in;

}






//compute transpose(A)*A
template <typename T>
void AtA(const mat<T>& a, mat<T>& ata)
{
	ata.resize(a.ncols(),a.ncols());
	ata.zeros();
	for(unsigned r = 0; r < a.nrows();r++)
	{
		for(unsigned i = 0; i < a.ncols();i++)
		{
			for(unsigned j = 0; j < a.ncols();j++)
			{
				ata(i,j)+=a(r,i)*a(r,j);
			}
		}
	}
}
//compute A*transpose(A)
template <typename T>
void AAt(const mat<T>& a, mat<T>& aat)
{
	aat.resize(a.nrows(),a.nrows());
	aat.zeros();
	for(unsigned c = 0; c < a.ncols();c++)
	{
		for(unsigned i = 0; i < a.nrows();i++)
		{
			for(unsigned j = 0; j < a.nrows();j++)
			{
				aat(i,j)+=a(i,c)*a(j,c);
			}
		}
	}
	
}

template <typename T>
mat<T> reshape(const vec<T>& v,unsigned r,unsigned c)
{
	assert(v.size() == r*c);
	return mat<T>(r,c,(const T*)v);
}

template <typename T>
mat<T> reshape(const mat<T>& m,unsigned r,unsigned c)
{
	assert(m.size() == r*c);
	return mat<T>(r,c,(const T*)m);
}


template <typename T>
void AtB(const mat<T>& a,const mat<T>& b, mat<T>& atb)
{
	atb.resize(a.ncols(),b.ncols());
	atb.zeros();
	
	for(unsigned i = 0; i < a.ncols(); i++)
		for(unsigned j = 0; j < b.ncols();j++)
			for(unsigned k = 0; k < a.nrows(); k++)
				atb(i,j) += a(k,i)*b(k,j); 
	
}

///multiply A^T*x 
/// A is a matrix and x is a vector
template <typename T>
void Atx(const mat<T>& a,const vec<T>& x, vec<T>& atx)
{
	atx.resize(a.ncols());
	atx.zeros();
	
	for(unsigned i = 0; i < a.ncols(); i++)
		for(unsigned j = 0; j < a.nrows(); j++)
			atx(i) += a(j,i) * (T)(x(j)); 
	
	
}


///returns the outer product of vector v and w
template < typename T, typename S>
mat<T> dyad(const vec<T>& v, const vec<S>& w) 
{ 
	mat<T> m(v.size(),w.size());

	for(unsigned i = 0; i < v.size();i++)
		for(unsigned j = 0; j < w.size();j++)
			m(i,j) =v(i)*(T)w(j); 
	
	return m;
}
