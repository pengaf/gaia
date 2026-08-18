#pragma once

#include "utility.h"

BEGIN_GAIA_MATH

template<typename Number_t>
class Vector2
{
public:
	Vector2() = default;
	Vector2(Number_t x, Number_t y) : x(x), y(y) {}
public:
	Number_t x{ 0 };
	Number_t y{ 0 };
};

template<typename Number_t>
class Vector3
{
public:
	Vector3() = default;
	Vector3(Number_t x, Number_t y, Number_t z) : x(x), y(y), z(z) {}
public:
	Number_t x{ 0 };
	Number_t y{ 0 };
	Number_t z{ 0 };
};

template<typename Number_t>
class Vector4
{
public:
	Vector4() = default;
	Vector4(Number_t x, Number_t y, Number_t z, Number_t w) : x(x), y(y), z(z), w(w) {}
public:
	Number_t x{ 0 };
	Number_t y{ 0 };
	Number_t z{ 0 };
	Number_t w{ 0 };
};

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;
using Vector2d = Vector2<double>;
using Vector3d = Vector3<double>;
using Vector4d = Vector4<double>;

END_GAIA_MATH