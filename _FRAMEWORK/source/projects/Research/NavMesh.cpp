#include "stdafx.h"
#include "NavMesh.h"

void NavMesh::AddTriangle(Geometry::VertexNode* v0, Geometry::VertexNode* v1, Geometry::VertexNode* v2)
{
	points.emplace(v0->index, v0->position);
	points.emplace(v1->index, v1->position);
	points.emplace(v2->index, v2->position);

	Geometry::Triangle triangle;
	triangle.points[0] = v0->index;
	triangle.points[1] = v1->index;
	triangle.points[2] = v2->index;
	triangles.emplace_back(std::move(triangle));
}

void NavMesh::DrawCircumference() const
{
	uint32_t size = static_cast<uint32_t>(points.size());
	for (uint32_t i = 1; i < size; ++i)
	{
		Elite::Vector2 pos1 = static_cast<Elite::Vector2>(points.at(i-1));
		Elite::Vector2 pos2 = static_cast<Elite::Vector2>(points.at(i));

		DEBUGRENDERER2D->DrawSegment(pos1, pos2, Elite::Color::OrangeRed);
	}

	Elite::Vector2 pos1 = static_cast<Elite::Vector2>(points.at(0));
	Elite::Vector2 pos2 = static_cast<Elite::Vector2>(points.at(size -1));
	DEBUGRENDERER2D->DrawSegment(pos1, pos2, Elite::Color::OrangeRed);
	
	for (const auto& it : points)
	{
		Elite::Vector2 renderpos = static_cast<Elite::Vector2>(it.second);
		DEBUGRENDERER2D->DrawPoint(renderpos, 3.f, Elite::Color::Blue);
	}
}

void NavMesh::DrawTriangles() const
{
	uint32_t size = static_cast<uint32_t>(triangles.size());
	for (uint32_t i = 1; i < size; ++i)
	{
		Elite::Vector2 pos0 = static_cast<Elite::Vector2>(points.at(triangles[i].points[0]));
		Elite::Vector2 pos1 = static_cast<Elite::Vector2>(points.at(triangles[i].points[1]));
		Elite::Vector2 pos2 = static_cast<Elite::Vector2>(points.at(triangles[i].points[2]));

		DEBUGRENDERER2D->DrawSegment(pos0, pos1, Elite::Color::BlushPink);
		DEBUGRENDERER2D->DrawSegment(pos1, pos2, Elite::Color::BlushPink);
		DEBUGRENDERER2D->DrawSegment(pos2, pos0, Elite::Color::BlushPink);
	}
}
