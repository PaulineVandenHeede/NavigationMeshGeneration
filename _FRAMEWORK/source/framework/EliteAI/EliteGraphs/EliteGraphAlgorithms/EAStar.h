#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
#pragma region PAULINE
		std::vector<T_NodeType*> path;
		std::vector<NodeRecord> openList; // uses operator < to sort
		std::vector<NodeRecord> closedList;

		//Kick off while loop
		NodeRecord start{  };
		start.pNode = pStartNode;
		start.pConnection = nullptr;
		start.costSoFar = 0.f;
		start.estimatedTotalCost = start.costSoFar + GetHeuristicCost(pStartNode, pGoalNode);
		
		openList.push_back(start);

		NodeRecord current{  };
		NodeRecord end{  };
		while (!openList.empty())
		{
			//connection with lowest fcost
			current = *std::min_element(openList.cbegin(), openList.cend());
			//current = openList.top();

			if (current.pNode == pGoalNode) break; //stop when goal is reached

			std::list<T_ConnectionType*> pConnections = m_pGraph->GetNodeConnections(current.pNode);
			for (T_ConnectionType* pConnection : pConnections)
			{
				end.pNode = m_pGraph->GetNode(pConnection->GetTo());
				//G-cost
				end.costSoFar = current.costSoFar + pConnection->GetCost();
				end.pConnection = pConnection;

				//Check if in closed list
				typename std::vector<NodeRecord>::const_iterator found = std::find_if(closedList.cbegin(), closedList.cend(), [&end](const NodeRecord& node) { return node.pNode == end.pNode; });
				if (found != closedList.cend())
				{
					if ((*found).costSoFar <= end.costSoFar)
						continue;

					closedList.erase(std::remove(closedList.begin(), closedList.end(), *found));
				}
				
				//Check if in open list
				found = std::find_if(openList.cbegin(), openList.cend(), [&end](const NodeRecord& node) { return node.pNode == end.pNode; });
				if (found != openList.cend())
				{
					if ((*found).costSoFar <= end.costSoFar)
						continue;

					openList.erase(std::remove(openList.begin(), openList.end(), *found));
				}

				//create new record
				end.estimatedTotalCost = end.costSoFar + GetHeuristicCost(current.pNode, pGoalNode);
				openList.push_back(end);
			}

			//remove current
			openList.erase(std::remove(openList.begin(), openList.end(), current));
			closedList.push_back(current);
		}

		//track back
		//current.pNode = pGoalNode; -> is already goal node
		while (current.pNode != pStartNode)
		{
			path.push_back(current.pNode);

			T_NodeType* pFrom = m_pGraph->GetNode(current.pConnection->GetFrom());

			typename std::vector<NodeRecord>::const_iterator it = std::find_if(closedList.cbegin(), closedList.cend(), [&pFrom](const NodeRecord& node) { return node.pNode == pFrom; });

			current = *it;
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());

		return path;
#pragma endregion PAULINE



	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}