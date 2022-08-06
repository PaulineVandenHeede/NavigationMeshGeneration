#pragma once
#include <array>
#include <vector>


namespace Geometry
{
	
	struct Edge
	{
		const Elite::Vector2* const p_begin_position;
		const Elite::Vector2* const p_end_position;

		explicit Edge(const Elite::Vector2* const begin_pos, const Elite::Vector2* const end_pos)
			: p_begin_position{ begin_pos }
			, p_end_position{ end_pos }
		{
		}

		Edge(const Edge& e) 
			: p_begin_position{ e.p_begin_position }
			, p_end_position{ e.p_end_position }
		{
		}
		Edge& operator=(const Edge& e) = delete;

		Edge(Edge&&) noexcept = delete;
		Edge& operator=(Edge&&) noexcept = delete;
	};

	struct DataStructure
	{
		Geometry::Edge edge;
		bool to_remove;

		DataStructure(const Edge& edge, bool to_remove = false)
			: edge{ edge }
			, to_remove{ to_remove }
		{
		}
	};

	struct SweepEvent
	{
		bool left;
		Elite::Vector2 position;
		SweepEvent* pOtherEventOnEdge;
	};



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

		[[nodiscard]] const std::vector<Elite::Vector2>& GetVertices() const;
		[[nodiscard]] const std::unordered_map<const Elite::Vector2*, Geometry::Edge>& GetEdges() const;

	private:
		void InsertBegin(const Elite::Vector2& v);
		void InsertEnd(const Elite::Vector2& v);

		std::vector<Elite::Vector2> points; //enforce no duplicate vertices
		std::unordered_map<const Elite::Vector2*, Geometry::Edge> edges; // can't have 2 points on the same place -> okay
		std::vector<Geometry::Triangle> triangles;
		std::vector<Geometry::Polygon> static_children;
	};

	void ScalePolygon(Geometry::Polygon& polygon);
	void MergePolygon(const std::vector<Geometry::Polygon*>& pPolygonstoMerge, Geometry::Polygon* pMergerdPolygon);

	bool LineLineIntersection(Elite::Vector2& intersection, const Elite::Vector2& p1, const Elite::Vector2& d1, const Elite::Vector2& p2, const Elite::Vector2& d2);
}

