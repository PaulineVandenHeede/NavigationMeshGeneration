#include "stdafx.h"
#include "HelperStructs.h"

#include <map>


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

		Geometry::Vertex vertex{ position, this };
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

		m_vertices.emplace_back(v, this);
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

			std::sort(edges.begin(), edges.end(), [](const Geometry::Edge& e1, const Geometry::Edge& e2)
				{
					const Elite::Vector2 center1 = (e1.begin_position + e1.end_position) / 2.f;
					const Elite::Vector2 center2 = (e2.begin_position + e2.end_position) / 2.f;
					return center1.y > center2.y;
				});
		}
	}


#pragma endregion !Polygon

	void ScalePolygon(Geometry::Polygon& polygon)
	{

	}
	void MergePolygon(const std::vector<Geometry::Polygon*>& p_polygons_to_merge, Geometry::Polygon* p_merged_polygon)
	{
		if (p_polygons_to_merge.empty())
		{
			//TODO: CHECK HOW TO PRINT OUT WARNINGS ETC WITH PLACE OF FILE > OVERLORD ENGINE
			return;
		}


		///STEP 1: SUBDIVIDING THE EDGES
		///		1.1: SORT VERTICES (FROM ALL POLYGONS) FROM LEFT TO RIGHT
		std::vector<Geometry::Vertex> vertices; //will hold copy of vertices of original polygons to make one big polygon
		std::vector<Geometry::Vertex> result_vertices;
		Geometry::Polygon* p_base_polygon = p_polygons_to_merge[0];

		//add all polygon vertices to one vector
		size_t size = p_polygons_to_merge.size();
		for(size_t i = 0; i < size; ++i)
		{
			const std::vector<Geometry::Vertex>& temp_vertices = p_polygons_to_merge[i]->GetVertices();
			std::ranges::copy(temp_vertices.cbegin(), temp_vertices.cend(), std::back_inserter(vertices));
		}

		std::ranges::sort(vertices.begin(), vertices.end(), [](const Geometry::Vertex& v0, const Geometry::Vertex& v1)
		{
			if (Elite::AreEqual(v0.position.x, v1.position.x))
			{
				return v0.position.y < v1.position.y;
			}
			return v0.position.x < v1.position.x;
		});
		
		///		1.2: SWEEP ALGORITHM
		std::deque<Geometry::DataStructure> data_structure_edges;
		std::vector<Geometry::SweepEvent> events;

		for (size_t i = 0; i < vertices.size(); ++i) //we need vertices.size() > because vector grows during the sweep algorithm
		{
			///A.	GET EDGES FROM VERTICES
			const Geometry::Vertex& current_vertex = vertices[i];
			auto potential_edges = current_vertex.edges;

			std::cout << current_vertex << std::endl;

			///B.	ADD EDGES TO DATA STRUCTURE
			const size_t previous_size = data_structure_edges.size();
			for(const Geometry::Edge& edge :potential_edges)
			{
				DataStructure data{ current_vertex, edge };
				data.p_polygon = current_vertex.p_polygon;
				data_structure_edges.emplace_front(data);
			}

			std::sort(data_structure_edges.begin(), data_structure_edges.end(), [](const DataStructure& d1, const DataStructure& d2)
				{
					Elite::Vector2 c1 = d1.edge.end_position - d1.edge.begin_position;
					Elite::Vector2 c2 = d2.edge.end_position - d2.edge.begin_position;

					if (Elite::AreEqual(c1.x, c2.x))
					{
						return c1.y < c2.y;
					}

					return c1.x < c2.x;
				});


			///C.	CHECK FOR COLLISION WITH EDGES FROM OTHER POLYGONS
			const size_t count = data_structure_edges.size() - previous_size;
			auto data_it = data_structure_edges.begin();
			
			for (size_t j = 0; j < count; ++j)
			{
				DataStructure data1 = data_structure_edges[j];
				const Geometry::Edge edge1 = data1.edge;

				data_it = std::find_if(data_structure_edges.begin() + j, data_structure_edges.end(), [&current_vertex](const DataStructure& data)
					{
						return data.p_polygon != current_vertex.p_polygon;
					});

				if (data_it == data_structure_edges.end())
					continue;

				const Geometry::Edge edge2 = (*data_it).edge;
				///		COLLISION ALGORITHM
				Elite::Vector2 direction1 = edge1.end_position - edge1.begin_position;
				Elite::Vector2 direction2 = edge2.end_position - edge2.begin_position;
				Elite::Vector2 intersection{};

				const bool result = Geometry::LineLineIntersection(intersection, data1.vertex.position, direction1, (*data_it).vertex.position, direction2);
				if (!result)
					continue;

				//Filter results of intersection
				// > the point of intersection cannot be a vertex that defines the edges
				// > if it is a point of one edge it automatically is a point of the other edge as well
				// > thus, it is enough to check one edge
				if ((Elite::AreEqual(intersection.x, edge1.begin_position.x, 0.00001f) && Elite::AreEqual(intersection.y, edge1.begin_position.y, 0.00001f))
					|| (Elite::AreEqual(intersection.x, edge1.end_position.x, 0.00001f) && Elite::AreEqual(intersection.y, edge1.end_position.y, 0.00001f)))
					continue;

				// > Intersection needs to lie on one of the edges
				if ((intersection.x < min(edge1.begin_position.x, edge1.end_position.x) || intersection.x > max(edge1.begin_position.x, edge1.end_position.x))
					|| (intersection.x < min(edge2.begin_position.x, edge2.end_position.x) || intersection.x > max(edge2.begin_position.x, edge2.end_position.x)))
					continue;
				if ((intersection.y < min(edge1.begin_position.y, edge1.end_position.y) || intersection.y > max(edge1.begin_position.y, edge1.end_position.y))
					|| (intersection.y < min(edge2.begin_position.y, edge2.end_position.y) || intersection.y > max(edge2.begin_position.y, edge2.end_position.y)))
					continue;

				///		ADD INTERSECTION AS NEW VERTEX TO VERTICES
				Geometry::Vertex intersection_vertex{ intersection, p_merged_polygon };
				intersection_vertex.intersection = true;
				//find element just after intersection
				auto found = std::find_if(vertices.cbegin(), vertices.cend(), [&intersection](const Geometry::Vertex& v)
					{
						return v.position.x > intersection.x;
					});
				auto inserted = vertices.insert(found, intersection_vertex);

				///		CHECK IF INSIDE OTHER POLYGON
				//start point 1
				Geometry::Vertex& begin_vertex1 = data_structure_edges[j].vertex;
				Geometry::Vertex& begin_vertex2 = data_structure_edges[j + 1].vertex;

				const bool result1 = PointInPolygon(begin_vertex1.position, begin_vertex2.p_polygon);
				begin_vertex1.start_outside_other_polygon = static_cast<e_outside_polygon>(result1);

				const bool result2 = PointInPolygon(begin_vertex2.position, begin_vertex1.p_polygon);
				begin_vertex2.start_outside_other_polygon = static_cast<e_outside_polygon>(result2);

#ifdef DEBUG
				std::cout << "Point " << edge1.begin_position << "is in polygon with point" << edge2.begin_position << " returned " << result1 << '\n';
				std::cout << "Point " << edge2.begin_position << "is in polygon with point" << edge1.begin_position << " returned " << result2 << '\n';
#endif

				///		CREATE NEW EDGES
				//Edge 1
				Geometry::Edge new_edge1_1{ edge1.begin_position, intersection };

				//Edge 2
				Geometry::Edge new_edge2_1{ edge2.begin_position, intersection };

				///		ADD EDGES TO NEW VERTEX
				if (!PointInPolygon(edge1.end_position, begin_vertex2.p_polygon))
				{
					Geometry::Edge new_edge1_2{ intersection, edge1.end_position };
					inserted->edges.emplace_back(new_edge1_2);
				}
				else
				{
					auto found_end = std::find_if(vertices.begin(), vertices.end(), [&edge1](Geometry::Vertex& vertex)
						{
							return vertex.position == edge1.end_position;
						});
					found_end->start_outside_other_polygon = e_outside_polygon::start_inside;
				}

				if (!PointInPolygon(edge2.end_position, begin_vertex1.p_polygon))
				{
					Geometry::Edge new_edge2_2{ intersection, edge2.end_position };
					inserted->edges.emplace_back(new_edge2_2);
				}
				else
				{
					auto found_end = std::find_if(vertices.begin(), vertices.end(), [&edge2](Geometry::Vertex& vertex)
						{
							return vertex.position == edge2.end_position;
						});
					found_end->start_outside_other_polygon = e_outside_polygon::start_inside;
				}

				inserted->start_outside_other_polygon = static_cast<e_outside_polygon>(!(result1 || result2));

				///		UPDATE EDGES OF CURRENT VERTEX
				//Edge 1
				auto vertex_it = std::find_if(vertices.begin(), vertices.end(), [&edge1](const Geometry::Vertex& v) { return edge1.begin_position == v.position; });
				auto it_edge = std::find_if(vertex_it->edges.begin(), vertex_it->edges.end(), [&vertex_it](const Geometry::Edge& edge) { return edge.begin_position == vertex_it->position; });
				vertex_it->start_outside_other_polygon = static_cast<e_outside_polygon>(result1);
				it_edge->end_position = intersection;

				//Edge 2
				vertex_it = std::find_if(vertices.begin(), vertices.end(), [&edge2](const Geometry::Vertex& v) { return edge2.begin_position == v.position; });
				it_edge = std::find_if(vertex_it->edges.begin(), vertex_it->edges.end(), [&vertex_it](const Geometry::Edge& edge) { return edge.begin_position == vertex_it->position; });
				vertex_it->start_outside_other_polygon = static_cast<e_outside_polygon>(result2);
				it_edge->end_position = intersection;

				///		ADD NEW EDGES TO DATA STRUCTURE
				//Edge 1
				DataStructure new_data_1{ data1.vertex, new_edge1_1 };
				DataStructure new_data_2{ (*data_it).vertex, new_edge2_1 };

				///		REMOVE ITEMS CORRESPONDING TO EDGES FROM DATA STRUCTURE
				
				data_structure_edges.erase(data_it);
				data_structure_edges.erase(data_structure_edges.begin() + j);

				//Edge 2
				data_structure_edges.insert(data_structure_edges.begin() + j, new_data_1);
				data_structure_edges.insert(data_structure_edges.begin() + j + 1, new_data_2);

			}

			///D.	REMOVE FROM DATA STRUCTURE THE EDGES THAT END
			erase_if(data_structure_edges, [&current_vertex](const DataStructure& d)
				{
					return d.edge.end_position == current_vertex.position;
				});

			
		}

		std::cout << "--- SUBDIVIDING EDGES DONE ---\n";

		///STEP 2: SELECTING THE EDGES
		///		2.1	ADD TO RESULT
		std::cout << vertices << std::endl;
		std::copy_if(vertices.cbegin(), vertices.cend(), std::back_inserter(result_vertices), [](const Vertex& vertex)
			{
				if(vertex.start_outside_other_polygon != e_outside_polygon::start_inside)
					return true;

				if (vertex.intersection)
					return true;
				
				return false;
			});

		std::for_each(result_vertices.begin(), result_vertices.end(), [](Vertex& vertex)
			{
				if(vertex.start_outside_other_polygon == e_outside_polygon::start_inside)
					vertex.edges.clear();
			});


		std::cout << result_vertices << std::endl;

		std::cout << "--- SELECTING THE EDGES DONE ---\n";

		///STEP 3 : MAKE FINAL CONTOUR
		p_merged_polygon->Reset();
		Geometry::Vertex* p_current_vertex = &result_vertices.front();
		do
		{
			//Process vertex
			p_current_vertex->processed = true;

			if(!p_current_vertex->edges.empty())
			{
				//take last edge and make endpoint the current_vertex
				Geometry::Edge& edge = p_current_vertex->edges.back();

				auto found = std::find_if(result_vertices.begin(), result_vertices.end(), [&edge](const Geometry::Vertex& v)
					{
						return v.position == edge.end_position && !v.processed;
					});

				if(found == result_vertices.cend())
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
				auto found = std::find_if(result_vertices.begin(), result_vertices.end(), [&p_current_vertex, &found_edge](const Geometry::Vertex& v)
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

				if(found == result_vertices.end())
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
				erase(found->edges, found_edge);

				p_current_vertex = &(*found);
			}
			
		} while (p_current_vertex != &result_vertices.front());




		std::cout << "---- END MERGING ALGORITHM ---\n";
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

}