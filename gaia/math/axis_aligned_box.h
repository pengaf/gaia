#pragma once

#include "vector.h"

BEGIN_GAIA

template<typename Vector_t>
class AxisAlignedBox
{
public:
	AxisAlignedBox() = default;
	AxisAlignedBox(const Vector_t& min, const Vector_t& max) : min(min), max(max)	{}
public:
	Vector_t min;
	Vector_t max;
};

using AxisAlignedBox2f = AxisAlignedBox<Vector2f>;
using AxisAlignedBox3f = AxisAlignedBox<Vector3f>;

END_GAIA