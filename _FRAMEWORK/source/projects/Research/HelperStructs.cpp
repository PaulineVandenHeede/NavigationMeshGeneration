#include "stdafx.h"
#include "HelperStructs.h"

#include <map>


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
		size_t size = vertices.size();
		points.reserve(size);
		for (const Elite::Vector2& v : vertices)
		{
			InsertEnd(v);
		}

		//edges.try_emplace(&points.front(), Geometry::Edge{ &points[0], &points[size - 1] });

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

	const std::vector<Elite::Vector2>& Polygon::GetVertices() const
	{
		return points;
	}

	const std::unordered_map< const Elite::Vector2*, Geometry::Edge>& Polygon::GetEdges() const
	{
		return edges;
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

		points.push_back(v);
		size_t size = points.size();
		if (size != 1)
		{
			const Elite::Vector2* reference = &points[size - 1];
			edges.emplace(&points[size-1], Geometry::Edge{&points[size - 1], &points[size - 2]});
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
#pragma endregion !Polygon

	void ScalePolygon(Geometry::Polygon& polygon)
	{

	}
	void MergePolygon(const std::vector<Geometry::Polygon*>& pPolygonsToMerge, Geometry::Polygon* pMergerdPolygon)
	{
		std::vector<int> numbers{ 0, 1, 2, 3, 4, 5 };
		int count = 6;
		for (int i{ 0 }; i < numbers.size(); ++i)
		{
			if(numbers[i] % 3 == 0)
			{
				std::vector<int>::iterator it = numbers.begin() + i;
				numbers.insert(it, count);
				++count;
			}
		}



		/////STEP 1: SUBDIVIDING THE EDGES
		/////		1.1: SORT VERTICES (FROM ALL POLYGONS) FROM LEFT TO RIGHT
		//std::vector<Elite::Vector2> vertices; //hold pointers to vertices because we need the original pointer for the edges
		//std::unordered_multimap<const Elite::Vector2*, const Geometry::Edge> edges;
		//size_t size = pPolygonsToMerge.size();
		//size_t total_size = 0;
		//for (size_t i = 0; i < size; ++i)
		//{
		//	total_size += pPolygonsToMerge[i]->GetVertices().size();
		//}

		////RESERVE MEMORY FOR VECTOR -> else memory addresses will be changed, we do not want that because we work with references (weak-pointers)
		//vertices.reserve(total_size);

		////add all polygon vertices to one vector
		////add all polygon edges to one map
		//for(size_t i = 0; i < size; ++i)
		//{
		//	const std::vector<Elite::Vector2>& tempVertices = pPolygonsToMerge[i]->GetVertices();
		//	const std::unordered_map<const Elite::Vector2*, Geometry::Edge>& tempEdges = pPolygonsToMerge[i]->GetEdges();

		//	//vertices
		//	std::for_each(tempVertices.cbegin(), tempVertices.cend(), [&vertices, &tempEdges, &edges](const Elite::Vector2& v)
		//		{
		//			vertices.push_back(v);

		//			//find edges corresponding to vertex
		//			const auto corresponding_edge = tempEdges.find(&v);
		//			edges.emplace(vertices.back(), Geometry::Edge{ corresponding_edge->second });
		//		}); //copy because we want to keep original polygons

		//	//edges
		//	std::vector<std::pair<const Elite::Vector2*, Elite::Line>> edges_to_add;
		//	//remove wrongly sorted edges and create correctly sorted edges
		//	std::erase_if(edges, [&edges_to_add, &vertices](const std::pair<const Elite::Vector2*, Elite::Line>& e)
		//		{
		//			//TODO: VERTICAL EDGES
		//			if (e.second.p2.x > e.second.p1.x) // not sorted correctly
		//			{
		//				//search in vector correct start point
		//				auto found = std::ranges::find_if(vertices.cbegin(), vertices.cend(), [&e](const Elite::Vector2& v)
		//					{
		//						return e.second.p1 == v;
		//					});
		//				Elite::Line line{ e.second.p2, e.second.p1 };
		//				edges_to_add.emplace_back(std::make_pair(e.first, line));
		//			}

		//			return e.second.p2.x > e.second.p1.x;
		//		});

		//	//add correct sorted edges
		//	std::ranges::for_each(edges_to_add.cbegin(), edges_to_add.cend(), [&edges](const std::pair<const Elite::Vector2*, Elite::Line>& edge)
		//		{
		//			edges.emplace(edge.first, edge.second);
		//		});


		//}

		////sort from left to right
		//std::ranges::sort(vertices.begin(), vertices.end(), [](const Elite::Vector2& v0, const Elite::Vector2& v1)
		//	{
		//		return(v0.x < v1.x);
		//	});

		/////		1.2: SWEEP ALGORITHM
		//size = vertices.size();
		//std::deque<Geometry::DataStructure> data_structure_edges;
		//std::vector<Geometry::SweepEvent> events;
		//for(size_t i = 0; i < size; ++i)
		//{
		//	///A.	GET EDGES FROM VERTICES
		//	const Elite::Vector2& vertex = vertices[i];
		//	auto potential_edges = edges.equal_range(&vertex);

		//	std::cout << "For vertex " << vertex << " found " << edges.count(&vertex) << " edges: \n";
		//	int j = 1;
		//	for(auto it = potential_edges.first; it != potential_edges.second; ++it)
		//	{
		//		std::cout << "\t edge " << std::to_string(j) << ": " << *(it->second.p_begin_position) << " - " << *(it->second.p_end_position) << '\n';
		//		++j;
		//	}

		//	///B.	ADD TO DATA STRUCTURE (holds the edges) THE EDGES THAT START
		//	const size_t previous_size = data_structure_edges.size();
		//	for (auto it = potential_edges.first; it != potential_edges.second; ++it)
		//	{
		//		DataStructure data{ it->second };
		//		data_structure_edges.emplace_front(data);
		//	}

		//	///C.	CHECK FOR COLLISION WITH NEIGHBOURING EDGES
		//	size_t nr_lines_to_check = data_structure_edges.size() - previous_size;
		//	for (size_t i = 0; i < nr_lines_to_check; ++i)
		//	{
		//		Geometry::Edge edge1 = data_structure_edges[i].edge;
		//		Geometry::Edge edge2 = data_structure_edges[i + 1].edge;

		//		///COLLISION ALGORITHM
		//		Elite::Vector2 direction1 = *edge1.p_end_position - *edge1.p_begin_position;
		//		Elite::Vector2 direction2 = *edge2.p_end_position - *edge2.p_begin_position;
		//		Elite::Vector2 intersection{};

		//		const bool result = Geometry::LineLineIntersection(intersection, *edge1.p_begin_position, direction1, *edge2.p_begin_position, direction2);
		//		if (!result)
		//			continue;

		//		///ADD INTERSECTION POINT TO VERTICES
		//		//find place in vertices, so that vertices stay sorted
		//		auto found = std::find_if(vertices.cbegin(), vertices.cend(), [&intersection](const Elite::Vector2* const v)
		//			{
		//				return v->x > intersection.x;
		//			});
		//		auto inserted = vertices.insert(found, intersection);

				///ADD NEW EDGES TO EDGES
				//{
				//	//Edge 1
				//	Geometry::Edge new_edge1_1{ &(*inserted), *edge1.p_end_position };
				//	Elite::Line new_edge1_2{ edge1.p2, intersection };

				//	found = std::find_if(vertices.cbegin(), vertices.cend(), [&edge1](const Elite::Vector2* const v)
				//		{
				//			return *v == edge1.p1;
				//		});

				//	edges.emplace(&(*found), new_edge1_1);
				//	edges.emplace(&(*inserted), new_edge1_2);

				//	//Edge 2
				//	Elite::Line new_edge2_1{ intersection, edge2.p1 };
				//	Elite::Line new_edge2_2{ edge2.p2, intersection };

				//	found = std::find_if(vertices.cbegin(), vertices.cend(), [&edge2](const Elite::Vector2* const v)
				//		{
				//			return *v == edge2.p1;
				//		});

				//	edges.emplace(&(*found), new_edge2_1);
				//	edges.emplace(&(*inserted), new_edge2_2);
				//}

				/////REMOVE INTERSECTING EDGES FROM DATA STRUCTURE
				//data_structure_edges[i].to_remove = true;
				//data_structure_edges[i + 1].to_remove = true;

				/////ADD NEW EDGES WITH CORRECT STARTING POINT TO DATA STRUCTURE AT POINT WHERE EDGES WERE REMOVED
				//DataStructure data1{};
				//data_structure_edges.emplace_front(data1)

				/////REMOVE OLD EDGES FROM EDGES

				/////MAKE 2 SWEEP EVENTS PER EDGE IF COLLIDED
				//SweepEvent event1_edge1{};
				//event1_edge1.position = intersection;

		//	}

			///D.	REMOVE FROM DATA STRUCTURE THE EDGES THAT END
			
			
			
		//}
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

}