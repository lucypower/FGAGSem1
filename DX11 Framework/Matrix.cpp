#include "Matrix.h"
#ifndef __MATRIX_CPP
#define __MATRIX_CPP

// parameter constructor
template <typename T>
Matrix<T>::Matrix(unsigned _rows, unsigned _columns, const T& _initial)
{
	mat.resize(_rows);
	//mat.resize(_columns);
	//mat.resize(_initial);

	// complete initialisation of member elements?

	rows = _rows;
	columns = _columns;
}

// copy constructor
template <typename T>
Matrix<T>::Matrix(const Matrix<T>& rhs)
{
	mat = rhs.mat;

	// complete to overwrite member elements
}

// (virtual) destructor
template <typename T>
Matrix<T>::~Matrix()
{

}

// assignment operator
template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs)
{
	if (&rhs == this)
	{
		return *this
	}
	// complete code stub

	return *this;
}

// addition of two matrices
template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& rhd)
{
	Matrix result(rows, columns, 0.0);

	// complete code stub

	return result;
}

// cummulative addition of this matric and another
template <typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& rhs)
{
	// complete code stub

	return *this;
}

// subtraction of two matrices
template <typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& rhd)
{
	Matrix result(rows, columns, 0.0);

	// complete code stub

	return result;
}

// cummulative subtraction of this matric and another
template <typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& rhs)
{
	// complete code stub

	return *this;
}

// left multiplication of two matrices
template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& rhd)
{
	Matrix result(rows, columns, 0.0);

	// complete code stub

	return result;
}

// cummulative left multiplication of this matric and another
template <typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& rhs)
{
	// complete code stub

	return *this;
}

// calculate a transpose of this matrix
template <typename T>
Matrix<T> Matrix<T>::transpose()
{
	// complete code stub

	return result;
}

// matrix / scalar addition
template <typename T>
Matrix<T> Matrix<T>::operator+(const T& rhs)
{
	// complete

	return result;
}

// matrix / scalar subtraction
template <typename T>
Matrix<T> Matrix<T>::operator-(const T& rhs)
{
	// complete

	return result;
}

// matrix / scalar multiplication
template <typename T>
Matrix<T> Matrix<T>::operator*(const T& rhs)
{
	// complete

	return result;
}

// matrix / scalar division
template <typename T>
Matrix<T> Matrix<T>::operator/(const T& rhs)
{
	// complete

	return result;
}

// multiply a matrix with a vector
template <typename T>
std::vector<T> Matrix<T>::operator*(const std::vector<T>& rhs)
{
	std::vector<T> result(rhs.size(), 0.0);

	// complete

	return result;
}

// obtain a vector of the diagonal elements
template <typename T>
std::vector<T> Matrix<T>::diag_vec()
{
	// complete

	return result;
}

// access the individial elements
template <typename T> 
T& Matrix<T>::operator() (const unsigned& row, const unsigned& column)
{
	return this->mat[row][column];
}

// access the individual elements (const)
template <typename T>
const T& Matrix<T>::operator() (const unsigned& row, const unsigned& column) const
{
	return this->mat[row][column];
}

// get the number of rows of the matrix
template <typename T>
unsigned Matrix<T>::get_rows() const
{
	return this->rows;
}

// get the number of columns of the matrix
template <typename T>
unsigned Matrix<T>::get_columns() const
{
	return this->columns;
}

#endif