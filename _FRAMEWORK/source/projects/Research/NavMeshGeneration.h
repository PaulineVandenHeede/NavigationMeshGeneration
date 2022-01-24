#pragma once

#include "HelperStructs.h"

class NavMesh;

class NavMeshGeneration
{
public:
	NavMeshGeneration() = default;
	~NavMeshGeneration() = default;

	NavMesh* CreateNavMesh(Geometry::Polygon bounds, std::initializer_list<Geometry::Polygon> obstacles);

private:

};

