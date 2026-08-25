#pragma once

#include "utility.h"

BEGIN_GAIA

class QuadtreeNode
{
protected:
	int m_x;
	int m_y;
};

class Quadtree
{
public:
	Quadtree(int8_t minLevel, int8_t max_Level);
	Quadtree(double radius, uint8_t levelCount);
protected:

};

END_GAIA
