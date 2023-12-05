#include "Matrix.hpp"

Matrix& Matrix::OrthographicProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	Matrix matrix = {};

	float a = right - left;
	float b = top - bottom;
	float c = farPlane - nearPlane;

	matrix.M11 = 2.0f / a;
	matrix.M12 = 0.0f;
	matrix.M13 = 0.0f;
	matrix.M14 = -(right + left) / a;

	matrix.M21 = 0.0f;
	matrix.M22 = 2.0f / b;
	matrix.M23 = 0.0f;
	matrix.M24 = -(top + bottom) / b;

	matrix.M31 = 0.0f;
	matrix.M32 = 0.0f;
	matrix.M33 = -2.0f / c;
	matrix.M34 = -(farPlane + nearPlane) / c;

	matrix.M41 = 0.0f;
	matrix.M42 = 0.0f;
	matrix.M43 = 0.0f;
	matrix.M44 = 1.0f;

	return matrix;
}

Matrix& Matrix::operator+(const Matrix& m)
{
	Matrix matrix = {};

	matrix.M11 = M11 + m.M11;
	matrix.M12 = M12 + m.M12;
	matrix.M13 = M13 + m.M13;
	matrix.M14 = M14 + m.M14;
	matrix.M21 = M21 + m.M21;
	matrix.M22 = M22 + m.M22;
	matrix.M23 = M23 + m.M23;
	matrix.M24 = M24 + m.M24;
	matrix.M31 = M31 + m.M31;
	matrix.M32 = M32 + m.M32;
	matrix.M33 = M33 + m.M33;
	matrix.M34 = M34 + m.M34;
	matrix.M41 = M41 + m.M41;
	matrix.M42 = M42 + m.M42;
	matrix.M43 = M43 + m.M43;
	matrix.M44 = M44 + m.M44;

	return matrix;
}

Matrix& Matrix::operator+=(const Matrix& m)
{
	Matrix matrix = {};

	matrix.M11 = M11 += m.M11;
	matrix.M12 = M12 += m.M12;
	matrix.M13 = M13 += m.M13;
	matrix.M14 = M14 += m.M14;
	matrix.M21 = M21 += m.M21;
	matrix.M22 = M22 += m.M22;
	matrix.M23 = M23 += m.M23;
	matrix.M24 = M24 += m.M24;
	matrix.M31 = M31 += m.M31;
	matrix.M32 = M32 += m.M32;
	matrix.M33 = M33 += m.M33;
	matrix.M34 = M34 += m.M34;
	matrix.M41 = M41 += m.M41;
	matrix.M42 = M42 += m.M42;
	matrix.M43 = M43 += m.M43;
	matrix.M44 = M44 += m.M44;

	return matrix;
}

Matrix& Matrix::operator-(const Matrix& m)
{
	Matrix matrix = {};

	matrix.M11 = M11 - m.M11;
	matrix.M12 = M12 - m.M12;
	matrix.M13 = M13 - m.M13;
	matrix.M14 = M14 - m.M14;
	matrix.M21 = M21 - m.M21;
	matrix.M22 = M22 - m.M22;
	matrix.M23 = M23 - m.M23;
	matrix.M24 = M24 - m.M24;
	matrix.M31 = M31 - m.M31;
	matrix.M32 = M32 - m.M32;
	matrix.M33 = M33 - m.M33;
	matrix.M34 = M34 - m.M34;
	matrix.M41 = M41 - m.M41;
	matrix.M42 = M42 - m.M42;
	matrix.M43 = M43 - m.M43;
	matrix.M44 = M44 - m.M44;

	return matrix;
}

Matrix& Matrix::operator-=(const Matrix& m)
{
	Matrix matrix = {};

	matrix.M11 = M11 -= m.M11;
	matrix.M12 = M12 -= m.M12;
	matrix.M13 = M13 -= m.M13;
	matrix.M14 = M14 -= m.M14;
	matrix.M21 = M21 -= m.M21;
	matrix.M22 = M22 -= m.M22;
	matrix.M23 = M23 -= m.M23;
	matrix.M24 = M24 -= m.M24;
	matrix.M31 = M31 -= m.M31;
	matrix.M32 = M32 -= m.M32;
	matrix.M33 = M33 -= m.M33;
	matrix.M34 = M34 -= m.M34;
	matrix.M41 = M41 -= m.M41;
	matrix.M42 = M42 -= m.M42;
	matrix.M43 = M43 -= m.M43;
	matrix.M44 = M44 -= m.M44;

	return matrix;
}

Matrix& Matrix::operator*(float f)
{
	Matrix matrix = {};

	matrix.M11 = M11 * f;
	matrix.M12 = M12 * f;
	matrix.M13 = M13 * f;
	matrix.M14 = M14 * f;
	matrix.M21 = M21 * f;
	matrix.M22 = M22 * f;
	matrix.M23 = M23 * f;
	matrix.M24 = M24 * f;
	matrix.M31 = M31 * f;
	matrix.M32 = M32 * f;
	matrix.M33 = M33 * f;
	matrix.M34 = M34 * f;
	matrix.M41 = M41 * f;
	matrix.M42 = M42 * f;
	matrix.M43 = M43 * f;
	matrix.M44 = M44 * f;

	return matrix;
}

Matrix& Matrix::operator*=(float f)
{
	Matrix matrix = {};

	matrix.M11 = M11 *= f;
	matrix.M12 = M12 *= f;
	matrix.M13 = M13 *= f;
	matrix.M14 = M14 *= f;
	matrix.M21 = M21 *= f;
	matrix.M22 = M22 *= f;
	matrix.M23 = M23 *= f;
	matrix.M24 = M24 *= f;
	matrix.M31 = M31 *= f;
	matrix.M32 = M32 *= f;
	matrix.M33 = M33 *= f;
	matrix.M34 = M34 *= f;
	matrix.M41 = M41 *= f;
	matrix.M42 = M42 *= f;
	matrix.M43 = M43 *= f;
	matrix.M44 = M44 *= f;

	return matrix;
}

Matrix& Matrix::operator*(const Matrix& m)
{
	Matrix matrix = {};

	matrix.M11 = M11 * m.M11 + M12 * m.M21 + M13 * m.M31 + M14 * m.M41;
	matrix.M12 = M11 * m.M12 + M12 * m.M22 + M13 * m.M32 + M14 * m.M42;
	matrix.M13 = M11 * m.M13 + M12 * m.M23 + M13 * m.M33 + M14 * m.M43;
	matrix.M14 = M11 * m.M14 + M12 * m.M24 + M13 * m.M34 + M14 * m.M44;

	matrix.M21 = M21 * m.M11 + M22 * m.M21 + M23 * m.M31 + M24 * m.M41;
	matrix.M22 = M21 * m.M12 + M22 * m.M22 + M23 * m.M32 + M24 * m.M42;
	matrix.M23 = M21 * m.M13 + M22 * m.M23 + M23 * m.M33 + M24 * m.M43;
	matrix.M24 = M21 * m.M14 + M22 * m.M24 + M23 * m.M34 + M24 * m.M44;

	matrix.M31 = M31 * m.M11 + M32 * m.M21 + M33 * m.M31 + M34 * m.M41;
	matrix.M32 = M31 * m.M12 + M32 * m.M22 + M33 * m.M32 + M34 * m.M42;
	matrix.M33 = M31 * m.M13 + M32 * m.M23 + M33 * m.M33 + M34 * m.M43;
	matrix.M34 = M31 * m.M14 + M32 * m.M24 + M33 * m.M34 + M34 * m.M44;

	matrix.M41 = M41 * m.M11 + M42 * m.M21 + M43 * m.M31 + M44 * m.M41;
	matrix.M42 = M41 * m.M12 + M42 * m.M22 + M43 * m.M32 + M44 * m.M42;
	matrix.M43 = M41 * m.M13 + M42 * m.M23 + M43 * m.M33 + M44 * m.M43;
	matrix.M44 = M41 * m.M14 + M42 * m.M24 + M43 * m.M34 + M44 * m.M44;

	return matrix;
}

Matrix& Matrix::operator*=(const Matrix& m)
{
	Matrix matrix = {};

	matrix.M11 += M11 * m.M11 + M12 * m.M21 + M13 * m.M31 + M14 * m.M41;
	matrix.M12 += M11 * m.M12 + M12 * m.M22 + M13 * m.M32 + M14 * m.M42;
	matrix.M13 += M11 * m.M13 + M12 * m.M23 + M13 * m.M33 + M14 * m.M43;
	matrix.M14 += M11 * m.M14 + M12 * m.M24 + M13 * m.M34 + M14 * m.M44;

	matrix.M21 += M21 * m.M11 + M22 * m.M21 + M23 * m.M31 + M24 * m.M41;
	matrix.M22 += M21 * m.M12 + M22 * m.M22 + M23 * m.M32 + M24 * m.M42;
	matrix.M23 += M21 * m.M13 + M22 * m.M23 + M23 * m.M33 + M24 * m.M43;
	matrix.M24 += M21 * m.M14 + M22 * m.M24 + M23 * m.M34 + M24 * m.M44;

	matrix.M31 += M31 * m.M11 + M32 * m.M21 + M33 * m.M31 + M34 * m.M41;
	matrix.M32 += M31 * m.M12 + M32 * m.M22 + M33 * m.M32 + M34 * m.M42;
	matrix.M33 += M31 * m.M13 + M32 * m.M23 + M33 * m.M33 + M34 * m.M43;
	matrix.M34 += M31 * m.M14 + M32 * m.M24 + M33 * m.M34 + M34 * m.M44;

	matrix.M41 += M41 * m.M11 + M42 * m.M21 + M43 * m.M31 + M44 * m.M41;
	matrix.M42 += M41 * m.M12 + M42 * m.M22 + M43 * m.M32 + M44 * m.M42;
	matrix.M43 += M41 * m.M13 + M42 * m.M23 + M43 * m.M33 + M44 * m.M43;
	matrix.M44 += M41 * m.M14 + M42 * m.M24 + M43 * m.M34 + M44 * m.M44;

	return matrix;
}
