#include "unit_mesh_source.h"
#include <map>

BEGIN_GAIA

std::strong_ordering operator<=>(const UnitMeshSource::UnitMeshParam& lhs, const UnitMeshSource::UnitMeshParam& rhs)
{
	std::strong_ordering cmp = lhs.type <=> rhs.type;
	if (cmp != std::strong_ordering::equal)
	{
		return cmp;
	}
	cmp = lhs.normal <=> rhs.normal;
	if (cmp != std::strong_ordering::equal)
	{
		return cmp;
	}
	cmp = lhs.texcoord <=> rhs.texcoord;
	if (cmp != std::strong_ordering::equal)
	{
		return cmp;
	}
	cmp = lhs.wire <=> rhs.wire;
	return cmp;
}

bool operator==(const UnitMeshSource::UnitMeshParam& lhs, const UnitMeshSource::UnitMeshParam& rhs)
{
	return lhs.type == rhs.type
		&& lhs.normal == rhs.normal
		&& lhs.texcoord == rhs.texcoord
		&& lhs.wire == rhs.wire;
}

struct CompareUnitMeshParamPtr
{
	bool operator()(const UnitMeshSource::UnitMeshParam* lhs, const UnitMeshSource::UnitMeshParam* rhs) const
	{
		if (lhs->type != rhs->type)
		{
			return lhs->type < rhs->type;
		}
		switch (lhs->type)
		{
		//case UnitMeshSource::UnitMeshType::square:
		//	return *lhs < *rhs;
		case UnitMeshSource::UnitMeshType::circle:
			return *static_cast<const UnitMeshSource::CircleParam*>(lhs) < *static_cast<const UnitMeshSource::CircleParam*>(rhs);
		}
		return false;
	}
};

class UnitMeshProvider
{
public:
	std::map<UnitMeshSource::UnitMeshParam*, Mesh*, CompareUnitMeshParamPtr> m_meshes;
};


UnitMeshSource::UnitMeshSource()
{
}

UnitMeshSource::~UnitMeshSource()
{
}

UnitMeshSource::UnitMeshType UnitMeshSource::unitMeshType() const
{
	return m_param ? m_param->type : UnitMeshType::none;
}

UnitMeshSource& UnitMeshSource::unitMeshType(UnitMeshType umt)
{
	return *this;
}

END_GAIA