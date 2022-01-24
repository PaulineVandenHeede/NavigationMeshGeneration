#include "stdafx.h"
#include "App_NavigationMeshGeneration.h"


App_NavigationMeshGeneration::~App_NavigationMeshGeneration()
{
	SAFE_DELETE(m_pStartPolygon);
}

void App_NavigationMeshGeneration::Start()
{
	//Elite::Polygon bounds = 
	Elite::Vertex v0{ Elite::Vector2{ 0.f, 0.f } };
	Elite::Vertex v1{ Elite::Vector2{ 2.f, 4.f } };
	Elite::Vertex v2{ Elite::Vector2{ 7.f, 0.f } };
	Elite::Vertex v3{ Elite::Vector2{ 13.f, 4.f } };
	Elite::Vertex v4{ Elite::Vector2{ 10.f, 10.f } };
	Elite::Vertex v5{ Elite::Vector2{ 6.f, 6.f } };
	Elite::Vertex v6{ Elite::Vector2{ 8.f, 7.f } };
	Elite::Vertex v7{ Elite::Vector2{ 9.f, 4.f } };
	Elite::Vertex v8{ Elite::Vector2{ 1.f, 6.f } };
	Elite::Vertex v9{ Elite::Vector2{ 0.f, 9.f } };
	Elite::Vertex v10{ Elite::Vector2{ 3.f, 11.f } };
	Elite::Vertex v11{ Elite::Vector2{ -2.f, 12.f } };
	Elite::Vertex v12{ Elite::Vector2{ -5.f, 10.f } };
	Elite::Vertex v13{ Elite::Vector2{ -4.f, 7.f } };
	Elite::Vertex v14{ Elite::Vector2{ -3.f, 9.f } };
	Elite::Vertex v15{ Elite::Vector2{ -1.f, 6.f } };
	Elite::Vertex v16{ Elite::Vector2{ -5.f, 3.f } };
	Elite::Vertex v17{ Elite::Vector2{ -1.f, 4.f } };

	m_pStartPolygon = new Geometry::Polygon{ { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17 } };
}

void App_NavigationMeshGeneration::Update(float deltaTime)
{

}

void App_NavigationMeshGeneration::Render(float deltaTime) const
{
	m_pStartPolygon->Draw();
}