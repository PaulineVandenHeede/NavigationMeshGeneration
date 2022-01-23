#ifndef NAVIGATION_MESH_GENERATION
#define NAVIGATION_MESH_GENERATION

#pragma once
#include "framework/EliteInterfaces/EIApp.h"

class App_NavigationMeshGeneration : public IApp
{
public:
	App_NavigationMeshGeneration() = default;
	virtual ~App_NavigationMeshGeneration();

	//App functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:

	App_NavigationMeshGeneration(const App_NavigationMeshGeneration&) = delete;
	App_NavigationMeshGeneration operator=(const App_NavigationMeshGeneration&) noexcept = delete;
};

#endif