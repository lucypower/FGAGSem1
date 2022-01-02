#pragma once
#ifndef __MATRIX_H
#define __MATRIX_H

#include <vector> // uses STL vector class


template <typename T> class Matrix
{
private:

	std::vector<std::vector<T>> mat; // vector of vectors = N columns of M rows
	unsigned rows;
	unsigned columns;

public:

	Matrix(unsigned _rows, unsigned _columns, const T& _initial); // any size
	Matrix(const Matrix<T>& rhs);
	virtual ~Matrix();

	// operator overloading, for standard mathematical matrix operations
	Matrix<T>& operator=(const Matrix<T>& rhs);

	// matrix mathematical operations
	Matrix<T> operator+(const Matrix<T>& rhs);
	Matrix<T>& operator+=(const Matrix<T>& rhs);
	Matrix<T> operator-(const Matrix<T>& rhs);
	Matrix<T>& operator-=(const Matrix<T>& rhs);
	Matrix<T> operator *(const Matrix<T>& rhs);
	Matrix<T>& operator *=(const Matrix<T>& rhs);
	Matrix<T> transpose();

	// matrix / scalar operations
	Matrix<T> operator+(const T& rhs);
	Matrix<T> operator-(const T& rhs);
	Matrix<T> operator*(const T& rhs);
	Matrix<T> operator/(const T& rhs);

	// matrix / vector operations
	std::vector<T> operator* (const std::vector<T>& rhs);
	std::vector<T> diag_vec();

	// access the individual elements
	T& operator() (const unsigned& row, const unsigned& column);
	const T& operator() (const unsigned& row, const unsigned& column) const;

	// access the row and column sizes
	unsigned get_rows() const;
	unsigned get_columns() const;
};
#include "Matrix.cpp" // included to allow template to refer to implementation
#endif