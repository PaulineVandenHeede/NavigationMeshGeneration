#ifndef NAVIGATION_MESH_GENERATION
#define NAVIGATION_MESH_GENERATION

#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "HelperStructs.h"

class NavMesh;

class App_NavigationMeshGeneration final: public IApp
{
public:
	App_NavigationMeshGeneration() = default;
	virtual ~App_NavigationMeshGeneration() override;

	//App functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	App_NavigationMeshGeneration(const App_NavigationMeshGeneration&) = delete;
	App_NavigationMeshGeneration operator=(const App_NavigationMeshGeneration&) noexcept = delete;

	//DataMembers
	std::vector<Geometry::Polygon*> m_pStartPolygons;
	std::vector<Geometry::Polygon*> m_pExpandedPolygons;
	Geometry::Polygon* m_pMergedPolygon;

	std::vector<Elite::Polygon*> m_pStartPolygonElites;
	std::vector<Elite::Polygon*> m_pExpandedPolygonElites;

	NavMesh* m_pNavMesh = nullptr;
};

#endif