#pragma once
#include <array>
#include <vector>

namespace Geometry
{
	enum class e_outside_polygon
	{
		start_outside = 0,
		start_inside = 1,
		start_unknown = 2,
	};

	struct Polygon;

	struct Edge
	{
		Elite::Vector2 begin_position;
		Elite::Vector2 end_position;

		Edge() = default;
		Edge(const Elite::Vector2& begin_pos, const Elite::Vector2& end_pos) : begin_position{ begin_pos }, end_position{ end_pos } {  }
		Edge(Elite::Vector2&& begin_pos, const Elite::Vector2&& end_pos) : begin_position{ begin_pos }, end_position{ end_pos } {  }

		Edge(const Geometry::Edge& e) = default;
		Edge(Geometry::Edge&& e) noexcept : begin_position{ e.begin_position }, end_position{ e.end_position } {  }

		Geometry::Edge& operator=(const Geometry::Edge& e)
		{
			if (this == &e)
				return *this;

			begin_position = e.begin_position;
			end_position = e.end_position;

			return *this;
		}
		Geometry::Edge& operator=(Geometry::Edge&& e) noexcept
		{
			begin_position = std::move(e.begin_position);
			end_position = std::move(e.end_position);

			return *this;
		}

		Elite::Vector2 Center() const { return (end_position + begin_position) / 2.f; }
	};

	inline bool operator==(const Geometry::Edge& e1, const Geometry::Edge& e2)
	{
		return(e1.begin_position == e2.begin_position && e1.end_position == e2.end_position);
	}

	struct Vertex
	{
		Elite::Vector2 position;
		Geometry::Polygon* p_polygon;
		std::vector<Geometry::Edge> edges;
		e_outside_polygon start_outside_other_polygon = e_outside_polygon::start_unknown;
		bool intersection = false;
		bool processed = false;
		char identifier = ' ';

		Vertex(const Elite::Vector2& pos, Geometry::Polygon* p_owner, char id) : position{ pos }, p_polygon{ p_owner }, edges{  }, identifier{ id } {  }
		Vertex(Elite::Vector2&& pos, Geometry::Polygon* p_owner, char id) : position{ pos }, p_polygon{ p_owner }, edges{  }, identifier{ id } {  }
	};



	inline bool operator==(const Geometry::Vertex& v1, const Geometry::Vertex& v2)
	{
		return v1.position == v2.position && v1.p_polygon == v2.p_polygon;
	}
	inline std::ostream& operator<<(std::ostream& os, const Geometry::Vertex& v)
	{
		os << "Vertex " << v.identifier << ' ' << v.position << " found " << v.edges.size() << " edges: \n";
		int j = 1;
		for (auto it = v.edges.cbegin(); it != v.edges.cend(); ++it)
		{
			os << "\t edge " << std::to_string(j) << ": " << it->begin_position << " - " << it->end_position << '\n';
			++j;
		}
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, const std::vector<Geometry::Vertex>& vertices)
	{
		os << "Vertices: \n";

		for(const Geometry::Vertex& v : vertices)
		{
			os << v;
		}

		return os;
	}


	struct DataStructure
	{
		Geometry::Vertex vertex;
		Geometry::Edge edge;
		Geometry::Polygon* p_polygon;
		bool to_be_removed = false;

		DataStructure(const Geometry::Vertex& vertex, const Geometry::Edge& edge) : vertex{ vertex }, edge{ edge } {  }
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
		static char next_id;

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
		void AddVertex(const Elite::Vector2& position, const Geometry::Edge& edge);
		void Expand(float distance) const;

		void Reset();
		void Copy(const Geometry::Polygon* p);

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


#pragma region BOOLEAN OPERATIONS ON POLYGONS
	void MergePolygons(const std::vector<Geometry::Polygon*>& pPolygonstoMerge, Geometry::Polygon* p_merged_polygon);
	void MergeTwoPolygons(const Geometry::Polygon* p_polygon_1, const Geometry::Polygon* p_polygon_2, Geometry::Polygon* p_merged_polygon);

	void SubtractPolygonsFromBasePolygon(const std::vector<Geometry::Polygon*>& p_polygons_to_substract, const Geometry::Polygon* p_base_polygon, Geometry::Polygon* p_result_polygon);

	bool LineLineIntersection(Elite::Vector2& intersection, const Elite::Vector2& p1, const Elite::Vector2& d1, const Elite::Vector2& p2, const Elite::Vector2& d2);
	bool PointInPolygon(const Elite::Vector2& point, const Geometry::Polygon* polygon);

	bool EvaluateEdgesForIntersection(Elite::Vector2& intersection, const Geometry::Edge& edge1, const Geometry::Edge& edge2);
	void AddEdgeToVertex(std::vector<Geometry::Vertex>& vertices, Geometry::Vertex& current_vertex, const Geometry::Vertex& comparing_vertex, const Geometry::Edge& edge);
	std::deque<Geometry::DataStructure>::iterator  InsertDataLexicographically(std::deque<Geometry::DataStructure>& data_structures, const Geometry::DataStructure& new_data);

	void DetermineIntersections(std::vector<Geometry::Vertex>& original_vertices);

	//different boolean operations
	void AND_BooleanOperation(std::vector<Geometry::Vertex>& original_vertices, std::vector<Geometry::Vertex>& result_vertices);
	void MINUS_BooleanOPeration(std::vector<Geometry::Vertex>& original_vertices, std::vector<Geometry::Vertex>& result_vertices);

	void MakeContour(std::vector<Geometry::Vertex>& result_vertices, Geometry::Polygon* p_merged_polygon);

#pragma endregion !BOOLEAN OPERATIONS ON POLYGONS
}

