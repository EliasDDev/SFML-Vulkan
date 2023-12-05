#pragma once

class Matrix
{
public:
	float M11 = 0.0f, M12 = 0.0f, M13 = 0.0f, M14 = 0.0f;
	float M21 = 0.0f, M22 = 0.0f, M23 = 0.0f, M24 = 0.0f;
	float M31 = 0.0f, M32 = 0.0f, M33 = 0.0f, M34 = 0.0f;
	float M41 = 0.0f, M42 = 0.0f, M43 = 0.0f, M44 = 0.0f;
public:
	Matrix& OrthographicProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane);
public:
	Matrix& operator+(const Matrix& m);
	Matrix& operator+=(const Matrix& m);

	Matrix& operator-(const Matrix& m);
	Matrix& operator-=(const Matrix& m);

	Matrix& operator*(float f);
	Matrix& operator*=(float f);

	Matrix& operator*(const Matrix& m);
	Matrix& operator*=(const Matrix& m);
};
