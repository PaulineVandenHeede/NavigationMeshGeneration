#include "stdafx.h"
#include "HelperStructs.h"

#include <map>

char Geometry::Polygon::next_id = 'A';


namespace Geometry
{
#pragma region VertexNode
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

#pragma endregion !VertexNode

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
		const size_t size = vertices.size();
		m_vertices.reserve(size);
		for (const Elite::Vector2& v : vertices)
		{
			InsertEnd(v);
		}

		Geometry::Vertex* reference = &m_vertices[size-1];
		reference->edges.emplace_back(Geometry::Edge{ reference->position, m_vertices[0].position });

		OrderEdges();
		SortEdgesOnCenterY();

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

		m_vertices = std::move(p.m_vertices);
		triangles = std::move(p.triangles);
		static_children = std::move(p.static_children);
	}

	Polygon& Polygon::operator=(Geometry::Polygon&& p) noexcept
	{
		pHead = p.pHead; p.pHead = nullptr;
		pEnd = p.pEnd; p.pEnd = nullptr;
		size = std::move(p.size);
		nextIndex = std::move(p.nextIndex);

		m_vertices = std::move(p.m_vertices);
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

	void Polygon::AddVertex(const Elite::Vector2& position, const Geometry::Edge& edge)
	{
		++size;
		++nextIndex;

		VertexNode* p_vertex = new VertexNode{ position };
		p_vertex->pNext = pHead;
		p_vertex->pPrevious = pEnd;
		p_vertex->index = nextIndex;

		if (pHead)
			pHead->pPrevious = p_vertex;
		else
			pHead = p_vertex;
		if (pEnd)
			pEnd->pNext = p_vertex;
		pEnd = p_vertex;

		++next_id;

		Geometry::Vertex vertex{ position, this, next_id };
		vertex.edges.emplace_back(edge);

		m_vertices.emplace_back(vertex);
	}

	void Polygon::Expand(float distance) const
	{
		std::vector<Elite::Vector2> newPositions{};

		VertexNode* p = pHead;
		for (uint64_t i = 0; i < size; ++i)
		{
			const VertexNode* const previous = p->pPrevious;
			VertexNode* const next = p->pNext;

			Elite::Vector2 edge1 = p->position - previous->position; //Already in correct opposite direction
			edge1.Normalize();
			Elite::Vector2 vp = Elite::Vector2{ edge1.y, -edge1.x };
			vp *= distance;

			Elite::Vector2 edge2 = p->position - next->position; //Already in correct opposite direction
			edge2.Normalize();
			Elite::Vector2 vn = Elite::Vector2{ -edge2.y, edge2.x };
			vn *= distance;

			//-- FORMULA EQUATION OF A LINE FROM POS & DISTANCE:
			//--		p + d
			//--			p = p.pos + vn or vp
			//--			d = edge1 or edge2
			Elite::Vector2 intersection{};
			const bool result = Geometry::LineLineIntersection(intersection, p->position + vp, edge1, p->position + vn, edge2);
			
			if(result)
			{
				newPositions.emplace_back(intersection);
				std::cout << "Vertex " << std::to_string(p->index) << '\n';
				std::cout << '\t' << "Old position" << p->position << '\n';
				std::cout << '\t' << "New position" << newPositions.back() << '\n';
				std::cout << std::endl;
			}

			p = next;
		}

		p = pHead;
		for (uint64_t i = 0; i < size; ++i)
		{
			p->position = newPositions[static_cast<uint32_t>(p->index)];
			p = p->pNext;
		}
	}

	void Polygon::Reset()
	{
		pHead = nullptr;
		pEnd = nullptr;
		size = 0;
		nextIndex = 0;

		m_vertices.clear();
		triangles.clear();
		static_children.clear();
	}

	void Polygon::Copy(const Geometry::Polygon* p)
	{
		VertexNode* p_other_vertex = p->pHead;

		do
		{
			++size;
			++nextIndex;

			VertexNode* p_new_vertex = new VertexNode{ p_other_vertex->position };
			p_new_vertex->pNext = pHead;
			p_new_vertex->pPrevious = pEnd;
			p_new_vertex->index = nextIndex;

			if (pHead)
				pHead->pPrevious = p_new_vertex;
			else
				pHead = p_new_vertex;

			if (pEnd)
				pEnd->pNext = p_new_vertex;

			pEnd = p_new_vertex;

			++next_id;

			Geometry::Vertex vertex{ p_new_vertex->position, this, next_id };
			m_vertices.emplace_back(vertex);

			const size_t size_vertices = m_vertices.size();
			if (size_vertices != 1)
			{
				Geometry::Vertex* reference = &m_vertices[size_vertices - 2];
				reference->edges.emplace_back(Geometry::Edge{ reference->position, vertex.position });
			}
			p_other_vertex = p_other_vertex->pNext;
		} while (p_other_vertex != p->pHead);

		Geometry::Vertex* reference = &m_vertices[size - 1];
		reference->edges.emplace_back(Geometry::Edge{ reference->position, m_vertices[0].position });
	}

	const std::vector<Geometry::Vertex>& Polygon::GetVertices() const
	{
		return m_vertices;
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

		m_vertices.emplace_back(v, this, next_id);
		++next_id;
		const size_t size_vertices = m_vertices.size();
		if (size_vertices != 1)
		{
			Geometry::Vertex* reference = &m_vertices[size_vertices - 2];
			reference->edges.emplace_back(Geometry::Edge{ reference->position, v });
		}

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
	void Polygon::OrderEdges()
	{
		size_t size = m_vertices.size();
		for(size_t i = 0; i < size; ++i)
		{
			std::vector<Geometry::Edge>& edges = m_vertices[i].edges;

			erase_if(edges, [this](const Geometry::Edge& edge)
				{
					if (edge.begin_position.x < edge.end_position.x)
						return false;

					auto it = std::find_if(m_vertices.begin(), m_vertices.end(), [&edge](const Geometry::Vertex& v) { return v.position == edge.end_position; });
					it->edges.emplace_back(std::move(edge.end_position), std::move(edge.begin_position));
					return true;
				});
		}
	}

	void Polygon::SortEdgesOnCenterY()
	{
		size_t size = m_vertices.size();
		for (size_t i = 0; i < size; ++i)
		{
			std::vector<Geometry::Edge>& edges = m_vertices[i].edges;

			std::ranges::sort(edges.begin(), edges.end(), [](const Geometry::Edge& e1, const Geometry::Edge& e2)
				{
					return e1.Center().y > e2.Center().y;
				});
		}
	}


#pragma endregion !Polygon

	void ScalePolygon(Geometry::Polygon& polygon)
	{

	}

	void MergePolygons(const std::vector<Geometry::Polygon*>& p_polygons_to_merge, Geometry::Polygon* p_merged_polygon)
	{
		if (p_polygons_to_merge.empty())
		{
			//TODO: CHECK HOW TO PRINT OUT WARNINGS ETC WITH PLACE OF FILE > OVERLORD ENGINE
			return;
		}

		if(p_polygons_to_merge.size() == 1)
		{
			//TODO: OR DO WE COPY > KEEP ORIGINAL POLYGON
			//SAFE_DELETE(p_merged_polygon);
			p_merged_polygon = p_polygons_to_merge[0];
		}

		const size_t size = p_polygons_to_merge.size();
		p_merged_polygon->Reset();
		p_merged_polygon->Copy(p_polygons_to_merge[0]);
		for(size_t i = 1; i < size; ++i)
		{
			MergeTwoPolygons(p_merged_polygon, p_polygons_to_merge[i], p_merged_polygon);

			std::cout << "---- TEMPORARY MERGED POLYGON ----\n";
			std::cout << p_merged_polygon->GetVertices() << std::endl;
		}
	}

	void MergeTwoPolygons(const Geometry::Polygon* p_polygon_1, const Geometry::Polygon* p_polygon_2, Geometry::Polygon* p_merged_polygon)
	{
		///STEP 1: SUBDIVIDING THE EDGES
		///		1.1: SORT VERTICES (FROM ALL POLYGONS) FROM LEFT TO RIGHT
		std::vector<Geometry::Vertex> vertices; //will hold copy of vertices of original polygons to make one big polygon
		std::vector<Geometry::Vertex> result_vertices;

		//add all polygon vertices to one vector
		const std::vector<Geometry::Vertex>& temp_vertices1 = p_polygon_1->GetVertices();
		std::ranges::copy(temp_vertices1.cbegin(), temp_vertices1.cend(), std::back_inserter(vertices));
		const std::vector<Geometry::Vertex>& temp_vertices2 = p_polygon_2->GetVertices();
		std::ranges::copy(temp_vertices2.cbegin(), temp_vertices2.cend(), std::back_inserter(vertices));


		std::ranges::sort(vertices.begin(), vertices.end(), [](const Geometry::Vertex& v0, const Geometry::Vertex& v1)
			{
				if (Elite::AreEqual(v0.position.x, v1.position.x))
				{
					return v0.position.y < v1.position.y;
				}
				return v0.position.x < v1.position.x;
			});

		DetermineIntersections(vertices);

#ifdef _DEBUG
		std::cout << "--- SUBDIVIDING EDGES DONE ---\n";
#endif

		///STEP 2: SELECTING THE EDGES
		///		2.1	ADD TO RESULT
#ifdef _DEBUG
		std::cout << vertices << std::endl;
#endif
		AND_BooleanOperation(vertices, result_vertices);

#ifdef _DEBUG
		std::cout << result_vertices << std::endl;
		std::cout << "--- SELECTING THE EDGES DONE ---\n";
#endif

		///STEP 3 : MAKE FINAL CONTOUR
		p_merged_polygon->Reset();
		MakeContour(result_vertices, p_merged_polygon);

#ifdef _DEBUG
		std::cout << "---- END MERGING ALGORITHM ---\n";
#endif 
	}

	void SubtractPolygonsFromBasePolygon(const std::vector<Geometry::Polygon*>& p_polygons_to_substract, const Geometry::Polygon* p_base_polygon, Geometry::Polygon* p_result_polygon)
	{
	}

	bool LineLineIntersection(Elite::Vector2& intersection, const Elite::Vector2& p1, const Elite::Vector2& d1, const Elite::Vector2& p2, const Elite::Vector2& d2)
	{
		//-- FORMULA EQUATION OF A LINE FROM POS & DISTANCE:
		//--		p + d

		//-- FORMULA CARTESIAN EQUATION:
		//--		a1 * X + b1 * Y + c1 = 0
		//--			a1 = d.y
		//--			b1 = -d.x
		//--			c1 = p.y * d.x - p.x * d.y

		//Parameters equation line 1
		const float a_1 = d1.y;
		const float b_1 = -d1.x;
		const float c_1 = (p1.y * d1.x) - (p1.x * d1.y);

		//Parameters equation line 2
		const float a_2 = d2.y;
		const float b_2 = -d2.x;
		const float c_2 = (p2.y * d2.x) - (p2.x * d2.y);

		//FIND INTERSECTION POINT
		//-- FORMULA	https://cp-algorithms.com/geometry/lines-intersection.html
		//--	x = - (c1 * b2 - c2 * b1) / (a1 * b2 - a2 * b1)
		//--	y = - (a1 * c2 - a2 * c1) / (a1 * b2 - a2 * b1)
		//-- REQUIREMENT a1 * b2 - a2 * b1 != 0 ELSE lines are parallel

		const float denominator = (a_1 * b_2) - (a_2 * b_1);

		if (Elite::AreEqual(denominator, 0.f))
			return false;

		const float numerator_x = (c_1 * b_2 - c_2 * b_1);
		const float numerator_y = (a_1 * c_2 - a_2 * c_1);

		intersection.x = -numerator_x / denominator;
		intersection.y = -numerator_y / denominator;
		return true;
	}

	bool PointInPolygon(const Elite::Vector2& point, const Geometry::Polygon* polygon)
	{
		const Elite::Vector2 direction{ 1,0 };

		const std::vector<Geometry::Vertex>& vertices = polygon->GetVertices();
		const size_t nr_vertices = vertices.size();

		size_t count = 0;
		for(size_t i = 0; i < nr_vertices; ++i)
		{
			const std::vector<Geometry::Edge>& edges = vertices[i].edges;
			const size_t nr_edges = edges.size();

			for (size_t j = 0; j < nr_edges; ++j)
			{
				Elite::Vector2 intersection{};
				bool result = LineLineIntersection(intersection, point, direction, edges[j].begin_position, edges[j].end_position - edges[j].begin_position);

				if (!result)
					continue;

				//no self intersection
				if ((Elite::AreEqual(intersection.x, edges[j].begin_position.x, 0.00001f) && Elite::AreEqual(intersection.y, edges[j].begin_position.y, 0.00001f))
					|| (Elite::AreEqual(intersection.x, edges[j].end_position.x, 0.00001f) && Elite::AreEqual(intersection.y, edges[j].end_position.y, 0.00001f)))
					continue;

				//only in positive x-direction
				if (intersection.x < point.x)
					continue;

				//only on edge
				if ((intersection.x < min(edges[j].begin_position.x, edges[j].end_position.x)) || (intersection.x > max(edges[j].begin_position.x, edges[j].end_position.x))
					|| (intersection.y < min(edges[j].begin_position.y, edges[j].end_position.y)) || (intersection.y > max(edges[j].begin_position.y, edges[j].end_position.y)))
					continue;

				++count;
			}
		}

		//if even > outside polygon
		//if uneven > inside polygon
		return !(count % 2 == 0);
	}

	bool EvaluateEdgesForIntersection(Elite::Vector2& intersection, const Geometry::Edge& edge1, const Geometry::Edge& edge2)
	{
		///		COLLISION ALGORITHM
		const Elite::Vector2 direction1 = edge1.end_position - edge1.begin_position;
		const Elite::Vector2 direction2 = edge2.end_position - edge2.begin_position;

		const bool result = Geometry::LineLineIntersection(intersection, edge1.begin_position, direction1, edge2.begin_position, direction2);
		if (!result)
			return false;

		//Filter results of intersection
		// > the point of intersection cannot be a vertex that defines the edges
		// > if it is a point of one edge it automatically is a point of the other edge as well
		// > thus, it is enough to check one edge
		if ((Elite::AreEqual(intersection.x, edge1.begin_position.x, 0.00001f) && Elite::AreEqual(intersection.y, edge1.begin_position.y, 0.00001f))
			|| (Elite::AreEqual(intersection.x, edge1.end_position.x, 0.00001f) && Elite::AreEqual(intersection.y, edge1.end_position.y, 0.00001f)))
			return false;

		// > Intersection needs to lie on one of the edges
		if ((intersection.x < min(edge1.begin_position.x, edge1.end_position.x) || intersection.x > max(edge1.begin_position.x, edge1.end_position.x))
			|| (intersection.x < min(edge2.begin_position.x, edge2.end_position.x) || intersection.x > max(edge2.begin_position.x, edge2.end_position.x)))
			return false;
		if ((intersection.y < min(edge1.begin_position.y, edge1.end_position.y) || intersection.y > max(edge1.begin_position.y, edge1.end_position.y))
			|| (intersection.y < min(edge2.begin_position.y, edge2.end_position.y) || intersection.y > max(edge2.begin_position.y, edge2.end_position.y)))
			return false;

		return true;
	}

	void AddEdgeToVertex(std::vector<Geometry::Vertex>& vertices, Geometry::Vertex& current_vertex, const Geometry::Vertex& comparing_vertex, const Geometry::Edge& edge)
	{
		current_vertex.edges.emplace_back(edge);
		if (PointInPolygon(edge.end_position, comparing_vertex.p_polygon))
		{
			auto found_end = std::find_if(vertices.begin(), vertices.end(), [&edge](Geometry::Vertex& vertex)
				{
					return vertex.position == edge.end_position;
				});
			//update original vertex
			found_end->start_outside_other_polygon = e_outside_polygon::start_inside;
		}
	}

	std::deque<Geometry::DataStructure>::iterator InsertDataLexicographically(std::deque<Geometry::DataStructure>& data_structures, const Geometry::DataStructure& new_data)
	{
		auto current_data_it = std::find_if(data_structures.begin(), data_structures.end(), [&new_data](const DataStructure& d)
			{
				Elite::Vector2 c1 = new_data.edge.Center();
				Elite::Vector2 c2 = d.edge.Center();

				if (Elite::AreEqual(c1.x, c2.x))
				{
					return c2.y < c1.y; //you want to insert it before the first one that is bigger;
				}

				return c2.x > c1.x; //if it doesn't find one that is equal then insert before the first one that is bigger on x.
			});

		current_data_it = data_structures.insert(current_data_it, new_data);

		return current_data_it;
	}

	void DetermineIntersections(std::vector<Geometry::Vertex>& original_vertices)
	{
		///		1.2: SWEEP ALGORITHM
		std::deque<Geometry::DataStructure> data_structure_edges;
		std::vector<Geometry::SweepEvent> events;

		for (size_t i = 0; i < original_vertices.size(); ++i) //we need vertices.size() > because vector grows during the sweep algorithm
		{
			///A.	GET EDGES FROM VERTICES
			const Geometry::Vertex& current_vertex = original_vertices[i];
			auto potential_edges = current_vertex.edges;

			///C.	REMOVE FROM DATA STRUCTURE THE EDGES THAT END
			erase_if(data_structure_edges, [&current_vertex, &original_vertices](const DataStructure& d)
				{
					return d.edge.end_position == current_vertex.position;
				});

#ifdef _DEBUG
			std::cout << current_vertex << std::endl;
#endif

			///B.	ADD EDGES TO DATA STRUCTURE
			std::vector<std::deque<Geometry::DataStructure>::iterator> potential_intersections;
			for (const Geometry::Edge& edge : potential_edges)
			{
				DataStructure current_data{ current_vertex, edge };
				current_data.p_polygon = current_vertex.p_polygon;


				auto current_data_it = InsertDataLexicographically(data_structure_edges, current_data);

				///D.	CHECK FOR COLLISION WITH EDGES FROM OTHER POLYGONS
				///they should be from other polygon

				auto test_data_it = data_structure_edges.begin();
				do
				{
					test_data_it = std::find_if(test_data_it, data_structure_edges.end(),
						[&current_data_it](const DataStructure& d) { return current_data_it->p_polygon != d.p_polygon; });

					if (test_data_it != data_structure_edges.end())
					{
						potential_intersections.emplace_back(test_data_it);
						++test_data_it; //else we don't move along the vector
					}

				} while (test_data_it != data_structure_edges.end());

				const size_t count = potential_intersections.size(); //only check for the new edges
				for (size_t j = 0; j < count; ++j)
				{
					const Geometry::Edge edge1 = current_data_it->edge;
					const Geometry::Edge edge2 = potential_intersections[j]->edge;
					Elite::Vector2 intersection{};

					const bool result = EvaluateEdgesForIntersection(intersection, edge1, edge2);

					if (!result)
						continue;

					///		ADD INTERSECTION AS NEW VERTEX TO VERTICES
					Geometry::Vertex intersection_vertex{ intersection, nullptr, Geometry::Polygon::next_id };
					++Geometry::Polygon::next_id;
					intersection_vertex.intersection = true;
					//find element just after intersection
					auto found = std::ranges::find_if(original_vertices.cbegin(), original_vertices.cend(), [&intersection](const Geometry::Vertex& v)
						{
							return v.position.x > intersection.x;
						});
					auto inserted = original_vertices.insert(found, intersection_vertex);

					///		CHECK IF INSIDE OTHER POLYGON
					Geometry::Vertex& begin_vertex1 = current_data_it->vertex;
					Geometry::Vertex& begin_vertex2 = potential_intersections[j]->vertex;

					const bool result1 = PointInPolygon(begin_vertex1.position, begin_vertex2.p_polygon);
					begin_vertex1.start_outside_other_polygon = static_cast<e_outside_polygon>(result1);

					const bool result2 = PointInPolygon(begin_vertex2.position, begin_vertex1.p_polygon);
					begin_vertex2.start_outside_other_polygon = static_cast<e_outside_polygon>(result2);

#ifdef _DEBUG
					std::cout << "Point " << edge1.begin_position << "is in polygon with point" << edge2.begin_position << " returned " << result1 << '\n';
					std::cout << "Point " << edge2.begin_position << "is in polygon with point" << edge1.begin_position << " returned " << result2 << '\n';
#endif

					///		SPLIT UP EDGES CORRESPONDING TO INTERSECTION
					Geometry::Edge new_edge1_1{ edge1.begin_position, intersection };
					Geometry::Edge new_edge1_2{ intersection, edge1.end_position };

					Geometry::Edge new_edge2_1{ edge2.begin_position, intersection };
					Geometry::Edge new_edge2_2{ intersection, edge2.end_position };

					///		ADD EDGES TO NEW VERTEX
					AddEdgeToVertex(original_vertices, *inserted, begin_vertex2, new_edge1_2);
					AddEdgeToVertex(original_vertices, *inserted, begin_vertex1, new_edge2_2);

					///		UPDATE CURRENT VERTEX
					inserted->start_outside_other_polygon = static_cast<e_outside_polygon>(!(result1 || result2));
					//Need to update original vertex
					//Edge 1
					auto vertex_it = std::ranges::find_if(original_vertices.begin(), original_vertices.end(), [&edge1](const Geometry::Vertex& v) { return edge1.begin_position == v.position; });
					auto it_edge = std::ranges::find_if(vertex_it->edges.begin(), vertex_it->edges.end(), [&vertex_it](const Geometry::Edge& edge) { return edge.begin_position == vertex_it->position; });
					vertex_it->start_outside_other_polygon = static_cast<e_outside_polygon>(result1);
					it_edge->end_position = intersection;
					//Edge 2
					vertex_it = std::ranges::find_if(original_vertices.begin(), original_vertices.end(), [&edge2](const Geometry::Vertex& v) { return edge2.begin_position == v.position; });
					it_edge = std::ranges::find_if(vertex_it->edges.begin(), vertex_it->edges.end(), [&vertex_it](const Geometry::Edge& edge) { return edge.begin_position == vertex_it->position; });
					vertex_it->start_outside_other_polygon = static_cast<e_outside_polygon>(result2);
					it_edge->end_position = intersection;

					///ADD NEW EDGES TO DATA STRUCTURE
					current_data_it->edge = new_edge1_1;
					potential_intersections[j]->edge = new_edge2_1;
				}
				potential_intersections.clear();
			}
		}
	}

	void AND_BooleanOperation(std::vector<Geometry::Vertex>& original_vertices,
		std::vector<Geometry::Vertex>& result_vertices)
	{
		std::ranges::copy_if(original_vertices.cbegin(), original_vertices.cend(), std::back_inserter(result_vertices), [](const Vertex& vertex)
			{
				if (vertex.start_outside_other_polygon != e_outside_polygon::start_inside)
					return true;

				if (vertex.intersection)
					return true;

				return false;
			});

		std::ranges::for_each(result_vertices.begin(), result_vertices.end(), [&original_vertices](Vertex& vertex)
			{
				if (vertex.intersection)
				{
					//check if it has edges that lead towards an inside vertex
					erase_if(vertex.edges, [&original_vertices](const Geometry::Edge& edge)
						{
							const auto it = std::ranges::find_if(original_vertices.cbegin(), original_vertices.cend(),
								[&edge](const Geometry::Vertex& vertex) { return edge.end_position == vertex.position; });
							if (it != original_vertices.cend())
							{
								return it->start_outside_other_polygon == e_outside_polygon::start_inside;
							}
							return false;
						});
				}
			});
	}

	void MINUS_BooleanOPeration(std::vector<Geometry::Vertex>& original_vertices, std::vector<Geometry::Vertex>& result_vertices)
	{

	}

	void MakeContour(std::vector<Geometry::Vertex>& result_vertices, Geometry::Polygon* p_merged_polygon)
	{
		Geometry::Vertex* p_current_vertex = &result_vertices.front();
		do
		{
			//Process vertex
			if (!p_current_vertex->edges.empty())
			{
				//take last edge and make endpoint the current_vertex
				Geometry::Edge& edge = p_current_vertex->edges.back();

				auto found = std::ranges::find_if(result_vertices.begin(), result_vertices.end(), [&edge](const Geometry::Vertex& v)
					{
						return v.position == edge.end_position;
					});

				if (found == result_vertices.cend())
				{
					//end algorithm
					Geometry::Edge e{ p_current_vertex->position, result_vertices.front().position };
					p_merged_polygon->AddVertex(p_current_vertex->position, e);

					p_current_vertex = &result_vertices.front();
					continue;
				}

				//add vertex
				p_merged_polygon->AddVertex(p_current_vertex->position, edge);
				p_current_vertex->edges.pop_back();

				p_current_vertex = &(*found);
			}
			else
			{
				//look in result vertices for point with that endpoint
				Geometry::Edge found_edge;
				auto found = std::ranges::find_if(result_vertices.begin(), result_vertices.end(), [&p_current_vertex, &found_edge](const Geometry::Vertex& v)
					{
						bool found = false;
						for (const Geometry::Edge& e : v.edges)
						{
							if (e.end_position == p_current_vertex->position)
							{
								found = true;
								found_edge = e;
							}
						}
						return found;
					});

				if (found == result_vertices.end())
				{
					//end algorithm
					Geometry::Edge e{ p_current_vertex->position, result_vertices.front().position };
					p_merged_polygon->AddVertex(p_current_vertex->position, e);

					p_current_vertex = &result_vertices.front();
					continue;
				}

				//add vertex
				Geometry::Edge added_edge{ found_edge.end_position, found_edge.begin_position };
				p_merged_polygon->AddVertex(p_current_vertex->position, added_edge);
				std::erase(found->edges, found_edge);

				p_current_vertex = &(*found);
			}

		} while (p_current_vertex != &result_vertices.front());
	}
}
