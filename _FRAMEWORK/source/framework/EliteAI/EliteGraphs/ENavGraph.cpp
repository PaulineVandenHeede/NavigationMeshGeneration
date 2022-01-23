#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	//2. Create connections now that every node is created
	//3. Set the connections cost to the actual distance

#pragma region OLD_SOLUTION
//	//loop over all lines
//	const std::vector<Elite::Line*> lines = m_pNavMeshPolygon->GetLines();
//	const int size = lines.size();
//	for (int i{ 0 }; i < size; ++i)
//	{
//		//connected to other triangle
//		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(lines[i]->index).size() <= 1)
//			continue;
//
//		//create graph node
//		Elite::Vector2 middle = (lines[i]->p1 + lines[i]->p2) / 2.f;
//		NavGraphNode* pNode = new NavGraphNode{ GetNextFreeNodeIndex() , lines[i]->index, middle };
//		AddNode(pNode);
//	}
//
//	//loop over all triangles
//	const std::vector<Elite::Triangle*> triangles = m_pNavMeshPolygon->GetTriangles();
//	const int sizeTriangles = triangles.size();
//	std::vector<int> nodeIndices;
//	nodeIndices.reserve(3);
//	int nodeIndex = invalid_node_index;
//	for (int i{ 0 }; i < sizeTriangles; ++i)
//	{
//		for (int index : triangles[i]->metaData.IndexLines)
//		{
//			nodeIndex = GetNodeIdxFromLineIdx(index);
//			if (IsNodeValid(nodeIndex)) nodeIndices.push_back(nodeIndex);
//		}
//
//		if (nodeIndices.size() == 2)
//		{
//			//create 1 connection
//			GraphConnection2D* pConnection = new GraphConnection2D{ nodeIndices[0], nodeIndices[1] };
//			AddConnection(pConnection);
//
//		}
//		else if (nodeIndices.size() == 3)
//		{
//			//create 3 connections
//			for (int i{ 0 }; i < 3; ++i)
//			{
//				int secondIndex = (i + 1) % 3;
//				GraphConnection2D* pConnection = new GraphConnection2D{ nodeIndices[i], nodeIndices[secondIndex] };
//				AddConnection(pConnection);
//			}
//		}
//
//		nodeIndices.clear();
//	}
//
#pragma endregion OLD_SOLUTION

	const std::vector<Elite::Triangle*> triangles = m_pNavMeshPolygon->GetTriangles();
	const int nrOfTriangles = triangles.size();
	
	std::vector<int> nodeIndices;
	nodeIndices.reserve(3);
	std::array<int, 3> triangleLinesIndices;

	int nodeIndex = invalid_node_index;
	for (int i{ 0 }; i < nrOfTriangles; ++i)
	{
		//create nodes + look for nodes where connections need to be made
		triangleLinesIndices = triangles[i]->metaData.IndexLines;
		for (int index : triangleLinesIndices)
		{
			nodeIndex = GetNodeIdxFromLineIdx(index);
			if (IsNodeValid(nodeIndex))
			{
				//node is already created
				nodeIndices.push_back(nodeIndex);
			}
			else
			{
				//Check if there is a node needed
				if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(index).size() <= 1)
					continue;

				//create graph node
				const Elite::Line* pLine = m_pNavMeshPolygon->GetLineFromLineIndex(index);
				Elite::Vector2 middle = (pLine->p1 + pLine->p2) / 2.f;
				NavGraphNode* pNode = new NavGraphNode{ GetNextFreeNodeIndex() , index, middle };
				AddNode(pNode);

				nodeIndices.push_back(pNode->GetIndex());
			}
		}

		//Create connections
		if (nodeIndices.size() == 2)
		{
			//create 1 connection
			GraphConnection2D* pConnection = new GraphConnection2D{ nodeIndices[0], nodeIndices[1] };
			AddConnection(pConnection);

		}
		else if (nodeIndices.size() == 3)
		{
			//create 3 connections
			for (size_t j{ 0 }; j < nodeIndices.size(); ++j)
			{
				int secondIndex = (j + 1) % nodeIndices.size();
				GraphConnection2D* pConnection = new GraphConnection2D{ nodeIndices[j], nodeIndices[secondIndex] };
				AddConnection(pConnection);
			}
		}

		nodeIndices.clear();
	}
	SetConnectionCostsToDistance();
}

