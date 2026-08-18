#pragma once

#include "vector.h"

BEGIN_GAIA_MATH

template<typename Number_t>
class Matrix2x2
{
public:
	Vector2<Number_t> columns[2];
};

template<typename Number_t>
class Matrix3x3
{
public:
	Vector3<Number_t> columns[3];
};

template<typename Number_t>
class Matrix4x4
{
public:
	Vector4<Number_t> columns[4];
};

template<typename Number_t>
class AffineMatrix2
{
public:
	Vector2<Number_t> columns[3];
};

template<typename Number_t>
class AffineMatrix3
{
public:
	Vector3<Number_t> columns[4];
};

template<typename LinearNumber_t, typename TranslationNumber_t = LinearNumber_t>
class AffineTransform2
{
public:
	AffineTransform2() = default;
	AffineTransform2(const Matrix3x3<LinearNumber_t>& linear, const Vector2<TranslationNumber_t>& translation) : linear(linear), translation(translation) {}
public:
	Matrix2x2<LinearNumber_t> linear;
	Vector2<TranslationNumber_t> translation;
};

template<typename LinearNumber_t, typename TranslationNumber_t = LinearNumber_t>
class AffineTransform3
{
public:
	AffineTransform3() = default;
	AffineTransform3(const Matrix4x4<LinearNumber_t>& linear, const Vector3<TranslationNumber_t>& translation) : linear(linear), translation(translation) {}
public:
	Matrix3x3<LinearNumber_t> linear;
	Vector3<TranslationNumber_t> translation;
};


using Matrix2x2f = Matrix2x2<float>;
using Matrix3x3f = Matrix3x3<float>;
using Matrix4x4f = Matrix4x4<float>;
using AffineMatrix2f = AffineMatrix2<float>;
using AffineMatrix3f = AffineMatrix3<float>;
using AffineTransform2f = AffineTransform2<float>;
using AffineTransform3f = AffineTransform3<float>;
using AffineTransform2fd = AffineTransform2<float, double>;
using AffineTransform3fd = AffineTransform3<float, double>;


#if GAIA_HIGH_PRECISION_POSITION_2D
using AffineTransform2fd = AffineTransform2D
#else
using AffineTransform2f = AffineTransform2D
#endif

#if GAIA_HIGH_PRECISION_POSITION_3D
using AffineTransform3fd = AffineTransform3D
#else
using AffineTransform3f = AffineTransform3D
#endif

END_GAIA_MATH