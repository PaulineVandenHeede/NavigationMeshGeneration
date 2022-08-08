#pragma once
#include <array>
#include <vector>


namespace Geometry
{
	struct Polygon;

	struct Edge
	{
		Elite::Vector2 begin_position;
		Elite::Vector2 end_position;

		Geometry::Polygon* p_polygon;

		Edge() = default;
		Edge(const Elite::Vector2& begin_pos, const Elite::Vector2& end_pos) : begin_position{ begin_pos }, end_position{ end_pos }, p_polygon{ nullptr } {  }
		Edge(Elite::Vector2&& begin_pos, const Elite::Vector2&& end_pos) : begin_position{ begin_pos }, end_position{ end_pos }, p_polygon{ nullptr } {  }
		Edge(Elite::Vector2& begin_pos, const Elite::Vector2& end_pos, Geometry::Polygon* p_owner) : begin_position{ begin_pos }, end_position{ end_pos }, p_polygon{ p_owner } {  }
		Edge(Elite::Vector2&& begin_pos, const Elite::Vector2&& end_pos, Geometry::Polygon* p_owner) : begin_position{ begin_pos }, end_position{ end_pos }, p_polygon{ p_owner } {  }

		Edge(const Geometry::Edge& e) = default;
		Edge(Geometry::Edge&& e) noexcept : begin_position{ e.begin_position }, end_position{ e.end_position }, p_polygon{ e.p_polygon } {  }

		Geometry::Edge& operator=(const Geometry::Edge& e)
		{
			begin_position = e.begin_position;
			end_position = e.end_position;
			p_polygon = e.p_polygon;

			return *this;
		}
		Geometry::Edge& operator=(Geometry::Edge&& e) noexcept
		{
			begin_position = std::move(e.begin_position);
			end_position = std::move(e.end_position);
			p_polygon = e.p_polygon; e.p_polygon = nullptr;

			return *this;
		}
	};

	inline bool operator==(const Geometry::Edge& e1, const Geometry::Edge& e2)
	{
		return(e1.begin_position == e2.begin_position && e1.end_position == e2.end_position);
	}

	struct Vertex
	{
		Elite::Vector2 position;
		std::vector<Geometry::Edge> edges;

		Vertex(const Elite::Vector2& pos) : position{ pos }, edges{  } {  }
		Vertex(Elite::Vector2&& pos) : position{ pos }, edges{  } {  }
	};

	inline std::ostream& operator<<(std::ostream& os, const Geometry::Vertex& v)
	{
		os << "Vertex " << v.position << " found " << v.edges.size() << " edges: \n";
		int j = 1;
		for (auto it = v.edges.cbegin(); it != v.edges.cend(); ++it)
		{
			os << "\t edge " << std::to_string(j) << ": " << it->begin_position << " - " << it->end_position << '\n';
			++j;
		}
		return os;
	}



	struct DataStructure
	{
		Geometry::Edge edge;

		DataStructure(const Geometry::Edge& edge)
			: edge{ edge }
		{
		}


	};

	struct SweepEvent
	{
		bool left;
		Elite::Vector2 position;
		SweepEvent* p_other_event_on_edge;
		Geometry::Polygon* p_polygon;
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

		[[nodiscard]] const std::vector<Geometry::Vertex>& GetVertices() const;

	private:
		void InsertBegin(const Elite::Vector2& v);
		void InsertEnd(const Elite::Vector2& v);
		void OrderEdges();
		void SortEdgesOnCenterY();

		std::vector<Geometry::Vertex> m_vertices; //enforce no duplicate vertices
		std::vector<Geometry::Triangle> triangles;
		std::vector<Geometry::Polygon> static_children;
	};

	void ScalePolygon(Geometry::Polygon& polygon);
	void MergePolygon(const std::vector<Geometry::Polygon*>& pPolygonstoMerge, Geometry::Polygon* pMergerdPolygon);

	bool LineLineIntersection(Elite::Vector2& intersection, const Elite::Vector2& p1, const Elite::Vector2& d1, const Elite::Vector2& p2, const Elite::Vector2& d2);
}

