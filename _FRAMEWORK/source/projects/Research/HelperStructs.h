#pragma once
#include <array>


namespace Geometry
{
	struct VertexNode
	{
		uint64_t index = 0;
		Elite::Vertex vertex;
		bool ear = false;
		VertexNode* pNext = nullptr, * pPrevious = nullptr;

		VertexNode() = default;
		VertexNode(Elite::Vertex vertex);
		VertexNode(float x, float y, float z = -0.5f);

		//copy semantics
		VertexNode(const VertexNode& v);
		VertexNode& operator=(const VertexNode& v);

		//move semantics
		VertexNode(VertexNode&& v) noexcept;
		VertexNode& operator=(VertexNode&& v) noexcept;
	};

	struct Polygon
	{
		//circular doubly linked list
		VertexNode* pHead = nullptr;
		VertexNode* pEnd = nullptr;
		uint64_t size = 0;
		uint64_t nextIndex = 0;

		Polygon() = default;
		Polygon(std::initializer_list<Elite::Vertex> vertices);
		~Polygon();

		void Draw() const;

	private:
		void InsertBegin(const Elite::Vertex& v);
		void InsertEnd(const Elite::Vertex& v);

	};
}

