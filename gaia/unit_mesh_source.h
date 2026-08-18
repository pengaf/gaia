#pragma once

#include "mesh_source.h"
#include <string>
#include <compare>


BEGIN_GAIA

namespace graphics
{
	class Mesh;
}
using Mesh = graphics::Mesh;

class UnitMeshSource : public MeshSource
{
public:
	enum class UnitMeshType : uint8_t
	{
		none,

		//2d
		square,
		circle,
		regular_polygon,

		//3d
		cube,
		sphere,
		cylinder,
		cone,
	};

	class UnitMeshParam : public pafcore::Object
	{
		UnitMeshType type;
		bool normal{ false };
		bool texcoord{ false };
		bool wire{ false };
		pafcore::ObserverPtr<UnitMeshSource> m_unitMeshSource;
	};

	class CircleParam : public UnitMeshParam
	{
		uint32_t segments{ 256 };

		std::strong_ordering operator<=>(const CircleParam& other) const
		{
			std::strong_ordering cmp = UnitMeshParam::operator<=>(other);
			if (cmp != std::strong_ordering::equal)
			{
				return cmp;
			}
			cmp = segments <=> other.segments;
			return cmp;
		}
		bool operator==(const CircleParam& other) const
		{
			return UnitMeshParam::operator==(other)
				&& segments == other.segments;
		}
	};

	class RegularPolygonParam : public UnitMeshParam
	{
		uint32_t segments{ 3 };
	};

	class SpherePolygonParam : public UnitMeshParam
	{
		uint32_t lonSegments{ 32 };
		uint32_t latSegments{ 16 };
	};

	class CylinderPolygonParam : public UnitMeshParam
	{
		uint32_t segments{ 256 };
	};

	class ConePolygonParam : public UnitMeshParam
	{
		uint32_t segments{ 256 };
	};

public:
	UnitMeshSource();
	~UnitMeshSource();
public:
	virtual RefPtr<Mesh> getMesh() = 0;
public:
	UnitMeshType unitMeshType() const;
	UnitMeshSource& unitMeshType(UnitMeshType umt);
private:
	pafcore::SharedPtr<UnitMeshParam> m_param;
};


END_GAIA