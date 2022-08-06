#include "stdafx.h"
#include "App_NavigationMeshGeneration.h"

#include "NavMeshGeneration.h"
#include "NavMesh.h"


App_NavigationMeshGeneration::~App_NavigationMeshGeneration()
{
	size_t size = m_pStartPolygons.size();
	for(size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_pStartPolygons[i]);
	}
	size = m_pExpandedPolygons.size();
	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_pExpandedPolygons[i]);
	}

	size = m_pStartPolygonElites.size();
	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_pStartPolygonElites[i]);
	}
	size = m_pExpandedPolygonElites.size();
	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_pExpandedPolygonElites[i]);
	}
}

void App_NavigationMeshGeneration::Start()
{
	//Elite::Polygon bounds =
	//-- TRY OUT: TRIANGULATION OF POLYGON --
	{
	/*Elite::Vertex v0{ Elite::Vector2{ 0.f, 0.f } };
	Elite::Vertex v1{ Elite::Vector2{ 10.f, 7.f } };
	Elite::Vertex v2{ Elite::Vector2{ 12.f, 3.f } };
	Elite::Vertex v3{ Elite::Vector2{ 20.f, 8.f } };
	Elite::Vertex v4{ Elite::Vector2{ 13.f, 17.f } };*/
	/*Elite::Vertex v5{Elite::Vector2{10.f, 12.f}};
	Elite::Vertex v6{ Elite::Vector2{ 12.f, 14.f } };
	Elite::Vertex v7{ Elite::Vector2{ 14.f, 9.f } };
	Elite::Vertex v8{ Elite::Vector2{ 8.f, 10.f } };
	Elite::Vertex v9{ Elite::Vector2{ 6.f, 14.f } };
	Elite::Vertex v10{ Elite::Vector2{ 10.f, 15.f } };
	Elite::Vertex v11{ Elite::Vector2{ 7.f, 18.f } };
	Elite::Vertex v12{ Elite::Vector2{ 0.f, 16.f } };
	Elite::Vertex v13{ Elite::Vector2{ 1.f, 13.f } };
	Elite::Vertex v14{ Elite::Vector2{ 3.f, 15.f } };
	Elite::Vertex v15{ Elite::Vector2{ 5.f, 8.f } };
	Elite::Vertex v16{ Elite::Vector2{ -2.f, 9.f } };
	Elite::Vertex v17{ Elite::Vector2{ -5.f, 5.f } };*/

	/*Elite::Vertex v0{ Elite::Vector2{ 10.f, 10.f } };
	Elite::Vertex v1{ Elite::Vector2{ 15.f, 0.f } };
	Elite::Vertex v2{ Elite::Vector2{ 10.f, -10.f } };
	Elite::Vertex v3{ Elite::Vector2{ -10.f, -10.f } };
	Elite::Vertex v4{ Elite::Vector2{ -10.f, 10.f } };*/

	//NavMeshGeneration generator{};
	//m_pNavMesh = generator.CreateNavMesh(*m_pStartPolygon, {});
	}

	// -- TRY OUT: EXPANDING POLYGON --
	{
		////-- Polygon 1: orthogonal
		///*Elite::Vector2 v0 = Elite::Vector2{ 2.f, -3.f };
		//Elite::Vector2 v1 = Elite::Vector2{ 6.f, -3.f };
		//Elite::Vector2 v2 = Elite::Vector2{ 6.f, 1.f };
		//Elite::Vector2 v3 = Elite::Vector2{ 8.f, 1.f };
		//Elite::Vector2 v4 = Elite::Vector2{ 8.f, -3.f };
		//Elite::Vector2 v5 = Elite::Vector2{ 12.f, -3.f };
		//Elite::Vector2 v6 = Elite::Vector2{ 12.f, 4.f };
		//Elite::Vector2 v7 = Elite::Vector2{ 2.f, 4.f };*/

		////-- Polygon 2: non-orthogonal
		//Elite::Vector2 v0 = Elite::Vector2{ 3.f, -2.f };
		//Elite::Vector2 v1 = Elite::Vector2{ 9.f, -2.f };
		//Elite::Vector2 v2 = Elite::Vector2{ 11.f, 3.f };
		//Elite::Vector2 v3 = Elite::Vector2{ 1.f, 3.f };

		//m_pStartPolygons.push_back(new Geometry::Polygon{ { v0, v1, v2, v3 } });
		//m_pExpandedPolygons.push_back(new Geometry::Polygon{ {v0, v1, v2, v3 } });
		//size_t size = m_pExpandedPolygons.size();
		//for (size_t i = 0; i < size; ++i)
		//{
		//	m_pExpandedPolygons[i]->Expand(2.f);
		//}

		//m_pStartPolygonElites.push_back(new Elite::Polygon{ { v0, v1, v2, v3 } });
		//m_pExpandedPolygonElites.push_back(new Elite::Polygon{ {v0, v1, v2, v3 } });
		//size = m_pExpandedPolygonElites.size();
		//for (size_t i = 0; i < size; ++i)
		//{
		//	m_pExpandedPolygonElites[i]->ExpandShape(-2.f);
		//}
	}


	//-- TRY OUT: POLYGON WITH HOLE --
	{
	/*Elite::Vector2 hv0 = Elite::Vector2{ 5.f, -1.f };
	Elite::Vector2 hv1 = Elite::Vector2{ 5.f, -1.f };
	Elite::Vector2 hv2 = Elite::Vector2{ 5.f, -1.f };
	Elite::Vector2 hv3 = Elite::Vector2{ 5.f, -1.f };*/

	//m_pStartPolygon->AddChild(Geometry::Polygon{ hv0, hv1, hv2, hv3 });
	}

	//-- TRY OUT: POLYGON MERGING --
	//- Polygon 1

	//-- TRY OUT: MERGE POLYGONS
	{
		Elite::Vector2 v0_0 = { 2.f, 4.f };
		Elite::Vector2 v0_1 = { 3.f, -4.f };
		Elite::Vector2 v0_2 = { 9.f, -4.f };
		Elite::Vector2 v0_3 = { 8.f, 4.f };

		//- Polygon 2
		Elite::Vector2 v1_0 = { 8.f, -3.f };
		Elite::Vector2 v1_1 = { 7.f, -10.f };
		Elite::Vector2 v1_2 = { 12.f, -10.f };
		Elite::Vector2 v1_3 = { 11.f, -3.f };

		m_pStartPolygons.push_back(new Geometry::Polygon{{ v0_0, v0_1, v0_2, v0_3 }});
		m_pStartPolygons.push_back(new Geometry::Polygon{ { v1_0, v1_1, v1_2, v1_3 } });
		m_pMergedPolygon = new Geometry::Polygon{};

		Geometry::MergePolygon(m_pStartPolygons, m_pMergedPolygon);
	}
}

void App_NavigationMeshGeneration::Update(float deltaTime)
{

}

void App_NavigationMeshGeneration::Render(float deltaTime) const
{
	//-- TRY OUT: EXPANDING OF POLYGON
	size_t size = min(m_pStartPolygonElites.size(), m_pExpandedPolygonElites.size()); 
	for(size_t i = 0; i < size; ++i)
	{
		DEBUGRENDERER2D->DrawPolygon(m_pStartPolygonElites[i], Elite::Color::AquaMenthe);
		DEBUGRENDERER2D->DrawPolygon(m_pExpandedPolygonElites[i], Elite::Color::Green);
	}

	size = min(m_pStartPolygons.size(), m_pExpandedPolygons.size());
	for(size_t i = 0; i < size; ++i)
	{
		m_pStartPolygons[i]->Draw();
		m_pExpandedPolygons[i]->Draw();
	}

	//-- TRY OUT: TRIANGULATION OF POLYGON
	//m_pNavMesh->DrawTriangles();
	//m_pNavMesh->DrawCircumference();
}