#include "stdafx.h"
#include "App_NavigationMeshGeneration.h"

#include "NavMeshGeneration.h"
#include "NavMesh.h"


App_NavigationMeshGeneration::~App_NavigationMeshGeneration()
{
	SAFE_DELETE(m_pStartPolygon);
}

void App_NavigationMeshGeneration::Start()
{
	//Elite::Polygon bounds = 
	Elite::Vertex v0{ Elite::Vector2{ 0.f, 0.f } };
	Elite::Vertex v1{ Elite::Vector2{ 10.f, 7.f } };
	Elite::Vertex v2{ Elite::Vector2{ 12.f, 3.f } };
	Elite::Vertex v3{ Elite::Vector2{ 20.f, 8.f } };
	Elite::Vertex v4{ Elite::Vector2{ 13.f, 17.f } };
	Elite::Vertex v5{ Elite::Vector2{ 10.f, 12.f } };
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
	Elite::Vertex v17{ Elite::Vector2{ -5.f, 5.f } };

	m_pStartPolygon = new Geometry::Polygon{ { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17 } };

	NavMeshGeneration generator{};
	NavMesh* pNavMesh = generator.CreateNavMesh(*m_pStartPolygon, {});
}

void App_NavigationMeshGeneration::Update(float deltaTime)
{

}

void App_NavigationMeshGeneration::Render(float deltaTime) const
{
	m_pStartPolygon->Draw();
}