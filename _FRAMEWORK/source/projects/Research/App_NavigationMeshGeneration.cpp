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

	// -- TRY OUT: POLYGON WITH HOLE --
	//Outside polygon
	Elite::Vector2 v0 = Elite::Vector2{ 2.f, -3.f };
	Elite::Vector2 v1 = Elite::Vector2{ 6.f, -3.f };
	Elite::Vector2 v2 = Elite::Vector2{ 6.f, 1.f };
	Elite::Vector2 v3 = Elite::Vector2{ 8.f, 1.f };
	Elite::Vector2 v4 = Elite::Vector2{ 8.f, -3.f };
	Elite::Vector2 v5 = Elite::Vector2{ 12.f, -3.f };
	Elite::Vector2 v6 = Elite::Vector2{ 12.f, 4.f };
	Elite::Vector2 v7 = Elite::Vector2{ 2.f, 4.f };

	/*Elite::Vector2 v0 = Elite::Vector2{ 3.f, -2.f };
	Elite::Vector2 v1 = Elite::Vector2{ 9.f, -2.f };
	Elite::Vector2 v2 = Elite::Vector2{ 11.f, 3.f };
	Elite::Vector2 v3 = Elite::Vector2{ 1.f, 3.f };*/



	//Hole
	Elite::Vector2 hv0 = Elite::Vector2{ 5.f, -1.f };
	Elite::Vector2 hv1 = Elite::Vector2{ 5.f, -1.f };
	Elite::Vector2 hv2 = Elite::Vector2{ 5.f, -1.f };
	Elite::Vector2 hv3 = Elite::Vector2{ 5.f, -1.f };



	m_pStartPolygon = new Geometry::Polygon{ { v0, v1, v2, v3, v4, v5, v6, v7 } };
	m_pExpandedPolygon = new Geometry::Polygon{ {v0, v1, v2, v3, v4, v5, v6, v7 } };
	m_pExpandedPolygon->Expand(2.f);

	m_pStartPolygonElite = new Elite::Polygon{ std::list<Elite::Vector2>{ v0, v1, v2, v3, v4, v5, v6, v7 } };
	m_pExpandedPolygonElite = new Elite::Polygon{ std::list<Elite::Vector2>{ v0, v1, v2, v3, v4, v5, v6, v7 } };
	m_pExpandedPolygonElite->ExpandShape(-2.f);


	//m_pStartPolygon->AddChild(Geometry::Polygon{ hv0, hv1, hv2, hv3 });


	//NavMeshGeneration generator{};
	//m_pNavMesh = generator.CreateNavMesh(*m_pStartPolygon, {});
}

void App_NavigationMeshGeneration::Update(float deltaTime)
{

}

void App_NavigationMeshGeneration::Render(float deltaTime) const
{
	//m_pStartPolygon->Draw();

	size_t size = std::min(m_pStartPolygonElites.size(), m_pExpandedPolygonElites.size()); 
	for(size_t i = 0; i < size; ++i)
	{
		DEBUGRENDERER2D->DrawPolygon(m_pStartPolygonElites, Elite::Color::AquaMenthe);
		DEBUGRENDERER2D->DrawPolygon(m_pExpandedPolygonElites, Elite::Color::Green);
	}

	m_pExpandedPolygon->Draw();

	//m_pNavMesh->DrawTriangles();
	//m_pNavMesh->DrawCircumference();
}