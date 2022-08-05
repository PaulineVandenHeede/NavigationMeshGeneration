#pragma once

#include "HelperStructs.h"

class NavMesh;

class NavMeshGeneration
{
public:
	NavMeshGeneration() = default;
	~NavMeshGeneration() = default;

	NavMesh* CreateNavMesh(Geometry::Polygon& bounds, std::initializer_list<Geometry::Polygon> obstacles, float playerRadius = 1.f);

private:

	void Triangulate(NavMesh* pNavMesh, Geometry::Polygon& bounds);
	void EarInit(Geometry::Polygon& bounds);
	bool Diagonal(Geometry::VertexNode* v0, Geometry::VertexNode* v1, Geometry::Polygon& bounds);
	bool InCone(Geometry::VertexNode* v0, Geometry::VertexNode* v1);
	bool Diagonalie(Geometry::VertexNode* v0, Geometry::VertexNode* v1, Geometry::Polygon& bounds);

	bool Left(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2);
	bool LeftOn(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2);
	bool Collinear(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2);
	bool Intersect(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2, Elite::Vector2& p3);
	bool IntersectProp(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2, Elite::Vector2& p3);
	int DoubleArea(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2);
	bool Between(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2);
	inline bool Xor(bool x, bool y) { return !x ^ !y; }
};

