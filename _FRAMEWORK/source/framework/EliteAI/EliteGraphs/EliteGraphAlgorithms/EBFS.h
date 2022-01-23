#pragma once

namespace Elite 
{
	template <class T_NodeType, class T_ConnectionType>
	class BFS
	{
	public:
		BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);
	private:
		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template <class T_NodeType, class T_ConnectionType>
	BFS<T_NodeType, T_ConnectionType>::BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> BFS<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode)
	{
		std::queue<T_NodeType*> openList; //Nodes we still have to check
		std::map<T_NodeType*, T_NodeType*> closedList; //Nodes we have checked & history of made connections

		openList.push(pStartNode); //Kickstart loop

		T_NodeType* pCurrentNode = nullptr;
		std::list<T_ConnectionType*> pConnections;
		while (!openList.empty())
		{
			pCurrentNode = openList.front();
			openList.pop();

			if (pCurrentNode == pDestinationNode)
				break; //stop while loop because end is found

			pConnections = m_pGraph->GetNodeConnections(pCurrentNode);

			std::for_each(pConnections.cbegin(), pConnections.cend(), [this, &openList, &closedList](T_ConnectionType* pConnection) {
				T_NodeType* pNextNode = m_pGraph->GetNode(pConnection->GetTo());
				if (closedList.find(pNextNode) == closedList.end())
				{
					//did not find node in closed list -> don't visit nodes twice
					openList.push(pNextNode);
					closedList[pNextNode] = pCurrentNode; //we came from this node
				}
				});

			//OTHER SOLUTION FOR FOREACH
			/*for (auto pConnection : m_pGraph->GetNodeConnections(pCurrentNode))
			{
				T_NodeType* pNextNode = m_pGraph->GetNode(pConnection->GetTo());
			}*/
		}

		//Track back
		std::vector<T_NodeType*> path;
		pCurrentNode = pDestinationNode;
		while (pCurrentNode != pStartNode)
		{
			path.push_back(pCurrentNode); //push front
			pCurrentNode = closedList[pCurrentNode];
		}
		path.push_back(pStartNode); //start node isn't nevessary (pos of agent) -> doesn't need to seek that position anymore

		//Reverse
		std::reverse(path.begin(), path.end());

		return path;

		
	}
}

