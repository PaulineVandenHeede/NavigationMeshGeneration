#include "stdafx.h"
#include "HelperStructs.h"

namespace Geometry
{
#pragma region Vertex
	VertexNode::VertexNode(Elite::Vector2 position)
		: index{ 0 }
		, position{ position }
		, ear{ false }
		, pNext{ nullptr }
		, pPrevious{ nullptr }
	{}
	VertexNode::VertexNode(float x, float y)
		: index{ 0 }
		, position{ x, y }
		, ear{ false }
		, pNext{ nullptr }
		, pPrevious{ nullptr }
	{}

	//copy semantics
	VertexNode::VertexNode(const VertexNode& v)
		: index{ v.index }
		, position{ v.position }
		, ear{ v.ear }
		, pNext{ v.pNext }
		, pPrevious{ v.pPrevious }
	{}
	VertexNode& VertexNode::operator=(const VertexNode& v)
	{
		index = v.index;
		position = v.position;
		ear = v.ear;
		pNext = v.pNext;
		pPrevious = v.pPrevious;

		return *this;
	}

	//Move semantics
	VertexNode::VertexNode(VertexNode&& v) noexcept
		: index{ std::move(v.index) }
		, position{ std::move(v.position) }
		, ear{ std::move(v.ear) }
		, pNext{ std::move(v.pNext) }
		, pPrevious{ std::move(v.pPrevious) }
	{
	}
	VertexNode& VertexNode::operator=(VertexNode&& v) noexcept
	{
		index = std::move(v.index);
		position = std::move(v.position);
		ear = std::move(v.ear);
		pNext = std::move(v.pNext);
		pPrevious = std::move(v.pPrevious);

		return *this;
	}

#pragma endregion !Vertex

#pragma region Triangle
	Triangle::Triangle(Triangle&& t) noexcept
	{
		for (int i = 0; i < 3; ++i)
		{
			points[i] = t.points[i];
			t.points[i] = 0;
		}
	}
	Triangle& Triangle::operator=(Triangle&& t) noexcept
	{
		for (int i = 0; i < 3; ++i)
		{
			points[i] = t.points[i];
			t.points[i] = 0;
		}
		return *this;
	}
#pragma endregion Triangle

#pragma region Polygon
	Polygon::Polygon(std::initializer_list<Elite::Vector2> vertices)
	{
		for (const Elite::Vector2& v : vertices)
		{
			InsertEnd(v);
		}

		std::cout << "Polygon constructor called!\n";
	}
	Polygon::~Polygon()
	{
		if (!pHead)
			return;

		while (pHead != pEnd) {
			VertexNode* tmp = pHead;
			pHead = pHead->pNext;
			delete tmp;
		}
		delete pHead;
	}

	Polygon::Polygon(Geometry::Polygon&& p) noexcept
	{
		pHead = p.pHead; p.pHead = nullptr;
		pEnd = p.pEnd; p.pEnd = nullptr;
		size = std::move(p.size);
		nextIndex = std::move(p.nextIndex);

		points = std::move(p.points);
		triangles = std::move(p.triangles);
		static_children = std::move(p.static_children);
	}

	Polygon& Polygon::operator=(Geometry::Polygon&& p) noexcept
	{
		pHead = p.pHead; p.pHead = nullptr;
		pEnd = p.pEnd; p.pEnd = nullptr;
		size = std::move(p.size);
		nextIndex = std::move(p.nextIndex);

		points = std::move(p.points);
		triangles = std::move(p.triangles);
		static_children = std::move(p.static_children);

		return *this;
	}


	void Polygon::Draw() const
	{
		VertexNode* temp = pHead;
		while (temp->pNext != pHead)
		{
			DEBUGRENDERER2D->DrawPoint(temp->position, 3.f, temp->colour);
			DEBUGRENDERER2D->DrawSegment(temp->position, temp->pNext->position, Elite::Color::OrangeRed);
			temp = temp->pNext;
		}
		DEBUGRENDERER2D->DrawPoint(temp->position, 3.f, temp->colour);
		DEBUGRENDERER2D->DrawSegment(temp->position, temp->pNext->position, Elite::Color::OrangeRed);
	}

	void Polygon::AddChild(Geometry::Polygon&& p)
	{
		static_children.push_back(p);
	}

	void Polygon::Expand(float distance) const
	{
		std::vector<Elite::Vector2> newPositions{};

		VertexNode* p = pHead;
		for (uint64_t i = 0; i < size; ++i)
		{
			VertexNode* previous = p->pPrevious;
			VertexNode* next = p->pNext;

			Elite::Vector2 vn = next->position - p->position; //dirTwo
			vn = Elite::Vector2{ vn.y, -vn.x }; //default is expanding not insetting -> that's why x has - and not y
			Elite::Normalize(vn);
			vn *= distance;

			Elite::Vector2 vp = p->position - previous->position; //dirOne
			vp = Elite::Vector2{ vp.y, -vp.x };
			Elite::Normalize(vp);
			vp *= distance;

			float bislen = distance / std::sqrtf(1 + vn.Dot(vp));

			Elite::Vector2 bis = vn + vp;
			//Elite::Normalize(bis);

			Elite::Vector2 vertex = p->position + bislen * bis;
			//Elite::Vector2 vertex = p->position + bis;

			newPositions.push_back(vertex);
			std::cout << "Vertex " << std::to_string(p->index) << '\n';
			std::cout << '\t' << "Old position" << p->position << '\n';
			std::cout << '\t' << "New position" << vertex << '\n';
			std::cout << std::endl;

			p = next;
		}

		p = pHead;
		for (uint64_t i = 0; i < size; ++i)
		{
			p->position = newPositions[static_cast<uint32_t>(p->index)];
			p = p->pNext;
		}
	}


	void Polygon::InsertBegin(const Elite::Vector2& v)
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
	void Polygon::InsertEnd(const Elite::Vector2& v)
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

	void ScalePolygon(Geometry::Polygon& polygon)
	{

	}
}