#include "stdafx.h"
#include "HelperStructs.h"

namespace Geometry
{
#pragma region Vertex
	VertexNode::VertexNode(Elite::Vertex vertex)
		: index{ 0 }
		, vertex{ vertex }
		, ear{ false }
		, pNext{ nullptr }
		, pPrevious{ nullptr }
	{}
	VertexNode::VertexNode(float x, float y, float z)
		: index{ 0 }
		, vertex{ Elite::Vector3{ x, y, z }, Elite::Color{ Elite::Color::AquaMenthe } }
		, ear{ false }
		, pNext{ nullptr }
		, pPrevious{ nullptr }
	{}

	//copy semantics
	VertexNode::VertexNode(const VertexNode& v)
		: index{ v.index }
		, vertex{ v.vertex }
		, ear{ v.ear }
		, pNext{ v.pNext }
		, pPrevious{ v.pPrevious }
	{}
	VertexNode& VertexNode::operator=(const VertexNode& v)
	{
		index = v.index;
		vertex = v.vertex;
		ear = v.ear;
		pNext = v.pNext;
		pPrevious = v.pPrevious;

		return *this;
	}

	//Move semantics
	VertexNode::VertexNode(VertexNode&& v) noexcept
		: index{ std::move(v.index) }
		, vertex{ std::move(v.vertex) }
		, ear{ std::move(v.ear) }
		, pNext{ std::move(v.pNext) }
		, pPrevious{ std::move(v.pPrevious) }
	{
	}
	VertexNode& VertexNode::operator=(VertexNode&& v) noexcept
	{
		index = std::move(v.index);
		vertex = std::move(v.vertex);
		ear = std::move(v.ear);
		pNext = std::move(v.pNext);
		pPrevious = std::move(v.pPrevious);

		return *this;
	}

#pragma endregion !Vertex

#pragma region Polygon
	Polygon::Polygon(std::initializer_list<Elite::Vertex> vertices)
	{
		for (const Elite::Vertex& v : vertices)
		{
			InsertEnd(v);
		}

		std::cout << "Polygon constructor called!\n";
	}
	Polygon::~Polygon()
	{
		while (pHead != pEnd) {
			VertexNode* tmp = pHead;
			pHead = pHead->pNext;
			delete tmp;
		}
		delete pHead;
	}

	void Polygon::Draw() const
	{
		VertexNode* temp = pHead;
		while (temp->pNext != pHead)
		{
			DEBUGRENDERER2D->DrawPoint(Elite::Vector2{ temp->vertex.position.x, temp->vertex.position.y }, 3.f, temp->vertex.color);
			DEBUGRENDERER2D->DrawSegment(Elite::Vector2{ temp->vertex.position.x, temp->vertex.position.y }, Elite::Vector2{ temp->pNext->vertex.position.x, temp->pNext->vertex.position.y }, Elite::Color::OrangeRed);
			temp = temp->pNext;
		}
		DEBUGRENDERER2D->DrawPoint(Elite::Vector2{ temp->vertex.position.x, temp->vertex.position.y }, 3.f, temp->vertex.color);
		DEBUGRENDERER2D->DrawSegment(Elite::Vector2{ temp->vertex.position.x, temp->vertex.position.y }, Elite::Vector2{ temp->pNext->vertex.position.x, temp->pNext->vertex.position.y }, Elite::Color::OrangeRed);
	}

	void Polygon::InsertBegin(const Elite::Vertex& v)
	{
		VertexNode* vertex = new VertexNode{ v };
		vertex->pNext = pHead;
		vertex->pPrevious = pEnd;
		vertex->index = nextIndex;

		if(pHead)
			pHead->pPrevious = vertex;
		if (pEnd)
			pEnd->pNext = vertex;
		else
			pEnd = vertex;
		pHead = vertex;

		++nextIndex;
		++size;
	}
	void Polygon::InsertEnd(const Elite::Vertex& v)
	{
		VertexNode* vertex = new VertexNode{ v };
		vertex->pNext = pHead;
		vertex->pPrevious = pEnd;
		vertex->index = nextIndex;

		if (pHead)
			pHead->pPrevious = vertex;
		else
			pHead = vertex;
		if(pEnd)
			pEnd->pNext = vertex;
		pEnd = vertex;

		++nextIndex;
		++size;
	}
#pragma endregion !Polygon
}