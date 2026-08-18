#pragma once

#include "vector.h"

BEGIN_GAIA_MATH

template<typename Number_t>
class OrientedBox2
{
public:
	typedef Vector2<Number_t> Vector_t
public:
	OrientedBox() = default;
	OrientedBox(const Vector_t& min, const Vector_t& max) : min(min), max(max) {}
public:
	Vector_t center;
	Vector_t extent;
	Vector_t uAxis;
	Vector_t vAxis;
};

template<typename Number_t>
class OrientedBox3
{
public:
	typedef Vector3<Number_t> Vector_t
public:
	OrientedBox() = default;
	OrientedBox(const Vector_t& min, const Vector_t& max) : min(min), max(max) {}
public:
	Vector_t center;
	Vector_t extent;
	Vector_t uAxis;
	Vector_t vAxis;
	Vector_t wAxis;
};


using OrientedBox2f = OrientedBox2<float>;
using OrientedBox3f = OrientedBox3<float>;

END_GAIA_MATH