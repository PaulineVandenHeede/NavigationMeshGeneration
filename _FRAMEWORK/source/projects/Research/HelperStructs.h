#pragma once
#include <array>


namespace Geometry
{
	struct VertexNode
	{
		uint64_t index = 0;
		Elite::Vector2 position;
		Elite::Color colour;
		bool ear = false;
		VertexNode* pNext = nullptr, * pPrevious = nullptr;

		VertexNode() = default;
		VertexNode(Elite::Vector2 vertex);
		VertexNode(float x, float y);

		//copy semantics
		VertexNode(const VertexNode& v);
		VertexNode& operator=(const VertexNode& v);

		//move semantics
		VertexNode(VertexNode&& v) noexcept;
		VertexNode& operator=(VertexNode&& v) noexcept;
	};

	struct Triangle
	{
		uint64_t points[3];

		Triangle() = default;
		~Triangle() = default;

		Triangle(const Triangle& t) = default;
		Triangle& operator=(const Triangle& t) = default;

		Triangle(Triangle&& t) noexcept;
		Triangle& operator=(Triangle&& t) noexcept;
	};

	struct Polygon
	{
		//circular doubly linked list
		VertexNode* pHead = nullptr;
		VertexNode* pEnd = nullptr;
		uint64_t size = 0;
		uint64_t nextIndex = 0;

		Polygon() = default;
		Polygon(std::initializer_list<Elite::Vector2> vertices);
		~Polygon();

		Polygon(const Geometry::Polygon& p) = default;
		Polygon& operator=(const Geometry::Polygon& p) = default;

		Polygon(Geometry::Polygon&& p) noexcept;
		Polygon& operator=(Geometry::Polygon&& p) noexcept;

		void Draw() const;
		void AddChild(Geometry::Polygon&& p);
		void Expand(float distance) const;

	private:
		void InsertBegin(const Elite::Vector2& v);
		void InsertEnd(const Elite::Vector2& v);

		std::vector<Elite::Vertex> points;
		std::vector<Geometry::Triangle> triangles;
		std::vector<Geometry::Polygon> static_children;
	};

	void ScalePolygon(Geometry::Polygon& polygon);
}

