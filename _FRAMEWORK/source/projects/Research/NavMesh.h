#pragma once

#include "HelperStructs.h"
#include <vector>

class NavMesh
{
public:
	NavMesh() = default;
	~NavMesh() = default;

	void AddTriangle(Geometry::VertexNode* v0, Geometry::VertexNode* v1, Geometry::VertexNode* v2);

	void DrawCircumference() const;
	void DrawTriangles() const;

private:
	std::unordered_map<uint64_t,Elite::Vertex> points;
	std::vector<Geometry::Triangle> triangles;
};

