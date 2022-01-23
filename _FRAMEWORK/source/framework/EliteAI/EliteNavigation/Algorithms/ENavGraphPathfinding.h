#pragma once
#include <vector>
#include <iostream>
#include "framework/EliteMath/EMath.h"
#include "framework\EliteAI\EliteGraphs\ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

namespace Elite
{
	class NavMeshPathfinding
	{
	public:
		static std::vector<Elite::Vector2> FindPath(Elite::Vector2 startPos, Elite::Vector2 endPos, Elite::NavGraph* pNavGraph, std::vector<Elite::Vector2>& debugNodePositions, std::vector<Elite::Portal>& debugPortals)
		{
			//Create the path to return
			std::vector<Elite::Vector2> finalPath{};

			//Get the start and endTriangle
			const Triangle* pStartTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos);
			const Triangle* pEndTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos);
			if (!pStartTriangle || !pEndTriangle) //Valid triangles
				return finalPath;
			if (pStartTriangle == pEndTriangle) //Same triangles
			{
				finalPath.push_back(endPos);
				return finalPath;
			}
		
			//We have valid start/end triangles and they are not the same
			//=> Start looking for a path
			//Copy the graph
			std::shared_ptr<Elite::IGraph<Elite::NavGraphNode, Elite::GraphConnection2D>> sp_copyGraph = pNavGraph->Clone();
			
			//Create extra node for the Start Node (Agent's position)
			Elite::NavGraphNode* pStartNode = new Elite::NavGraphNode(sp_copyGraph->GetNextFreeNodeIndex(), invalid_node_index, startPos); //line index -1 -> not situated on a line
			sp_copyGraph->AddNode(pStartNode);
			//create connections to node
			int nodeIndex = invalid_node_index;
			for (int lineIndex : pStartTriangle->metaData.IndexLines)
			{
				nodeIndex = pNavGraph->GetNodeIdxFromLineIdx(lineIndex);
				if (pNavGraph->IsNodeValid(nodeIndex))
				{
					float distance = Elite::Distance(startPos, pNavGraph->GetNodePos(nodeIndex));
					Elite::GraphConnection2D* pConnection = new Elite::GraphConnection2D{ pStartNode->GetIndex(), nodeIndex, distance };

					sp_copyGraph->AddConnection(pConnection);
				}
			}
			
			//Create extra node for the endNode
			Elite::NavGraphNode* pEndNode = new Elite::NavGraphNode(sp_copyGraph->GetNextFreeNodeIndex(), invalid_node_index, endPos);
			sp_copyGraph->AddNode(pEndNode);
			//create connections to node
			for (int lineIndex : pEndTriangle->metaData.IndexLines)
			{
				nodeIndex = pNavGraph->GetNodeIdxFromLineIdx(lineIndex);
				if (pNavGraph->IsNodeValid(nodeIndex))
				{
					float distance = Elite::Distance(endPos, pNavGraph->GetNodePos(nodeIndex));
					Elite::GraphConnection2D* pConnection = new Elite::GraphConnection2D{ pEndNode->GetIndex(), nodeIndex, distance };

					sp_copyGraph->AddConnection(pConnection);
				}
			}

			
			//Run A star on new graph
			Elite::Heuristic pHeuristicFunction = Elite::HeuristicFunctions::Manhattan;
			auto pathfinder = AStar<NavGraphNode, GraphConnection2D>(sp_copyGraph.get(), pHeuristicFunction);
			std::vector<NavGraphNode*> path = pathfinder.FindPath(pStartNode, pEndNode);

			debugNodePositions.clear();
			std::for_each(path.cbegin(), path.cend(), [&finalPath, &debugNodePositions](const NavGraphNode* pNode) {
				finalPath.push_back(pNode->GetPosition());
				debugNodePositions.push_back(pNode->GetPosition());
				});
			
			//OPTIONAL BUT ADVICED: Debug Visualisation

			//Run optimiser on new graph, MAKE SURE the A star path is working properly before starting this section and uncommenting this!!!
			debugPortals = SSFA::FindPortals(path, pNavGraph->GetNavMeshPolygon());
			finalPath = SSFA::OptimizePortals(debugPortals);

			return finalPath;
		}
	};
}
