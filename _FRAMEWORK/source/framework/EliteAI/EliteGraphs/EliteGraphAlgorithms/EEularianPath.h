#pragma once
#include <stack>

namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	template <class T_NodeType, class T_ConnectionType>
	class EulerianPath
	{
	public:

		EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		Eulerianity IsEulerian() const;
		vector<T_NodeType*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(int startIdx, vector<bool>& visited) const;
		bool IsConnected() const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template<class T_NodeType, class T_ConnectionType>
	inline EulerianPath<T_NodeType, T_ConnectionType>::EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline Eulerianity EulerianPath<T_NodeType, T_ConnectionType>::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected())
			return Eulerianity::notEulerian;

		// Count nodes with odd degree 
		std::vector<T_NodeType*> activeNodes = m_pGraph->GetAllActiveNodes();
		int oddcount = std::count_if(activeNodes.cbegin(), activeNodes.cend(), [this](T_NodeType* pNode) { return (m_pGraph->GetNodeConnections(pNode).size() & 1); });

		//-> A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		//-> A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		//   An Euler trail can be made, but only starting and ending in these 2 nodes
		//-> A connected graph with no odd nodes is Eulerian
		switch (oddcount)
		{
		case 2:
			if (activeNodes.size() != 2)
				return Eulerianity::semiEulerian;
			else
				return Eulerianity::notEulerian;
		case 0:
			return Eulerianity::eulerian;
		default:
			return Eulerianity::notEulerian;
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	inline vector<T_NodeType*> EulerianPath<T_NodeType, T_ConnectionType>::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		auto path = vector<T_NodeType*>();
		int nrOfNodes = graphCopy->GetNrOfNodes();
		int location = invalid_node_index;

		// Check if there can be an Euler path
		// If this graph is not eulerian, return the empty path
		// Else we need to find a valid starting index for the algorithm
		// A	Even degree nodes -> choose randomly
		// B	2 nodes odd degree -> choose 1
		// C	else no euler path
		switch (eulerianity)
		{
		case Eulerianity::notEulerian:
			return path;
		case Eulerianity::semiEulerian:
			for (int i{0}; i < nrOfNodes; ++i)
			{
				//node valid & odd degree connections
				if (m_pGraph->IsNodeValid(i) && (m_pGraph->GetNodeConnections(i).size() & 1))
				{
					location = i;
					break;
				}
			}
			break;
		case Eulerianity::eulerian:
		default:
			location = 0;
			break;
		}


		// Start algorithm loop
		stack<int> nodeStack;
		list<T_ConnectionType*> connections{};
		while (location != invalid_node_index)
		{
			connections = graphCopy->GetNodeConnections(location);
			if (connections.size() != 0)
			{
				if (graphCopy->IsNodeValid(connections.front()->GetTo()))
				{
					nodeStack.push(location);
					location = connections.front()->GetTo();
					graphCopy->RemoveConnection(connections.front());
				}

				continue;
			}

			path.push_back(graphCopy->GetNode(location));

			if (!nodeStack.empty())
			{
				location = nodeStack.top();
				nodeStack.pop();
			}
			else
			{
				location = invalid_node_index;
			}
		}

		std::reverse(path.begin(), path.end());

		return path;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline void EulerianPath<T_NodeType, T_ConnectionType>::VisitAllNodesDFS(int startIdx, vector<bool>& visited) const
	{
		// mark the visited node
		visited[startIdx] = true;

		// recursively visit any valid connected nodes that were not visited before
		std::list<T_ConnectionType*> connections = m_pGraph->GetNodeConnections(startIdx);
		std::for_each(connections.cbegin(), connections.cend(), [&visited, this](T_ConnectionType* pConnection) {
			int nodeIndex = pConnection->GetTo();
			if (pConnection->IsValid() && !visited[nodeIndex])
				VisitAllNodesDFS(nodeIndex, visited);
			});
	}

	template<class T_NodeType, class T_ConnectionType>
	inline bool EulerianPath<T_NodeType, T_ConnectionType>::IsConnected() const
	{
		std::vector<T_NodeType*> activeNodes = m_pGraph->GetAllActiveNodes();
		std::vector<bool> visited(m_pGraph->GetNrOfNodes(), false);

		// find a valid starting node that has connections
		int startIndex = invalid_node_index;
		for (T_NodeType* pNode : activeNodes)
		{
			std::list<T_ConnectionType*> connections = m_pGraph->GetNodeConnections(pNode);
			if (connections.size() != 0)
			{
				startIndex = pNode->GetIndex();
				break;
			}
		}
		
		// if no valid node could be found, return false
		if (startIndex == invalid_node_index)
			return false;

		// start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(startIndex, visited);

		// if a node was never visited, this graph is not connected
		for (T_NodeType* pNode : activeNodes)
		{
			if (visited[pNode->GetIndex()] == false)
				return false;
		}

		return true;
	}

}