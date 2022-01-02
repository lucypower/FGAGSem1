#include "Vector3D.h"

Vector3D::Vector3D() // constructor
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3D::Vector3D(float x1, float y1, float z1) // constructor with values
{
	x = x1;
	y = y1;
	z = z1;
}

Vector3D::Vector3D(const Vector3D& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

// addition

Vector3D Vector3D::operator+(const Vector3D& vec)
{
	// returns a new vector summing the values for each component with the corresponding component in the added vector
	return Vector3D(x + vec.x, y + vec.y, z + vec.z);
}

Vector3D& Vector3D::operator+=(const Vector3D& vec)
{
	// returns 'this' pointer (self-reference summing the values for each component with the corresponding component in the added vector)
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

// subtraction

Vector3D Vector3D::operator-(const Vector3D& vec)
{
	return Vector3D(x - vec.x, y - vec.y, z - vec.z);
}

Vector3D& Vector3D::operator-=(const Vector3D& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

// multiplication

Vector3D Vector3D::operator*(float value)
{
	return Vector3D(x * value, y * value, z * value);
}

Vector3D& Vector3D::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

// division

Vector3D Vector3D::operator/(float value)
{
	assert(value != 0); // prevents divide by 0
	return Vector3D(x / value, y / value, z / value);
}

Vector3D& Vector3D::operator /=(float value)
{
	assert(value != 0);

	x /= value;
	y /= value;
	z /= value;

	return *this;
}

// =

Vector3D& Vector3D::operator=(const Vector3D& vec)
{
	return Vector3D(x = vec.x, y = vec.y, z = vec.z);
}

// dot product

//float Vector3D::dot_product(const Vector3D& vec)
//{
//	//return ((x1 * x2) + (y1 * y2) + (z1 * z2));
//}

// cross product

//Vector3D Vector3D::cross_product(const Vector3D& vec)
//{
//	float ni = y * vec.z - z * vec.y;
//
//	return vec;
//}

//float Vector3D::magnitude()
//{
//	float sqrt = square();
//	return sqrt;
//}

float Vector3D::square()
{
	return x * x + y * y + z * z;
}

//Vector3D Vector3D::normalization()
//{
//	
//}

//float Vector3D::distance(const Vector3D& vec)
//{
//
//}

float Vector3D::show_X()
{
	return x;
}

float Vector3D::show_Y()
{
	return y;
}

float Vector3D::show_Z()
{
	return z;
}

void Vector3D::disp()
{
	std::cout << x << " " << y << y << " " << z << std::endl;
}