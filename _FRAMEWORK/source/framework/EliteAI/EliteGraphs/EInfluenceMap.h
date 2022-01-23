#pragma once
#include "EIGraph.h"
#include "EGraphNodeTypes.h"
#include "EGraphConnectionTypes.h"

namespace Elite
{
	template<class T_GraphType>
	class InfluenceMap final : public T_GraphType
	{
	public:
		InfluenceMap(bool isDirectional): T_GraphType(isDirectional) {}
		void InitializeBuffer() { m_InfluenceDoubleBuffer = vector<float>(m_Nodes.size()); }
		void PropagateInfluence(float deltaTime);

		void SetInfluenceAtPosition(Elite::Vector2 pos, float influence);

		void Render() const {}
		void SetNodeColorsBasedOnInfluence();

		float GetMomentum() const { return m_Momentum; }
		void SetMomentum(float momentum) { m_Momentum = momentum; }

		float GetDecay() const { return m_Decay; }
		void SetDecay(float decay) { m_Decay = decay; }

		float GetPropagationInterval() const { return m_PropagationInterval; }
		void SetPropagationInterval(float propagationInterval) { m_PropagationInterval = propagationInterval; }

	protected:
		virtual void OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged) override;

	private:
		Elite::Color m_NegativeColor{ 1.f, 0.2f, 0.f};
		Elite::Color m_NeutralColor{ 0.f, 0.f, 0.f };
		Elite::Color m_PositiveColor{ 0.f, 0.2f, 1.f};

		float m_MaxAbsInfluence = 100.f;

		float m_Momentum = 0.8f; // a higher momentum means a higher tendency to retain the current influence
		float m_Decay = 0.1f; // determines the decay in influence over distance

		float m_PropagationInterval = .05f; //in Seconds
		float m_TimeSinceLastPropagation = 0.0f;

		vector<float> m_InfluenceDoubleBuffer;
	};

	template <class T_GraphType>
	void InfluenceMap<T_GraphType>::PropagateInfluence(float deltaTime)
	{
		// TODO: implement

		//1.	Only execute once every interval
		m_TimeSinceLastPropagation += deltaTime;
		if (m_TimeSinceLastPropagation < m_PropagationInterval)
			return;

		m_TimeSinceLastPropagation -= m_PropagationInterval;

		//TODO BUFFER
		InitializeBuffer();

		//2.	Go over all nodes
		float highestInfluence = 0.f;
		float newInfluence = 0.f;
		float neighbourCurrentInfluence = 0.f;
		NodeVector pNodes = GetAllActiveNodes();
		for (NodeType* pNode : pNodes)
		{
			ConnectionList pConnections = GetNodeConnections(pNode);
			for (ConnectionType* pConnection : pConnections)
			{
				int neighbour = pConnection->GetTo();
				if (!IsNodeValid(neighbour))
					continue;
				
				neighbourCurrentInfluence = GetNode(neighbour)->GetInfluence();
				newInfluence = neighbourCurrentInfluence * expf(-pConnection->GetCost() * m_Decay);

				if (abs(newInfluence) > abs(highestInfluence))
					highestInfluence = newInfluence;
			}

			//Calculate new influence
			highestInfluence = Clamp(highestInfluence, -m_MaxAbsInfluence, m_MaxAbsInfluence);
			highestInfluence = Lerp<float>(pNode->GetInfluence(), highestInfluence, m_Momentum);
			m_InfluenceDoubleBuffer[pNode->GetIndex()] = highestInfluence;

			highestInfluence = FLT_MIN;
		}

		uint32_t size = m_InfluenceDoubleBuffer.size();
		for (uint32_t i{ 0 }; i < size; ++i)
		{
			if(IsNodeValid(i))
				GetNode(i)->SetInfluence(m_InfluenceDoubleBuffer[i]);
		}

		//m_InfluenceDoubleBuffer.clear();
	}

	template <class T_GraphType>
	inline void InfluenceMap<T_GraphType>::SetInfluenceAtPosition(Elite::Vector2 pos, float influence)
	{
		auto idx = GetNodeIdxAtWorldPos(pos);
		if (IsNodeValid(idx))
			GetNode(idx)->SetInfluence(influence);
	}

	template<class T_GraphType>
	inline void InfluenceMap<T_GraphType>::SetNodeColorsBasedOnInfluence()
	{
		const float half = .5f;

		for (auto& pNode : m_Nodes)
		{
			Color nodeColor{};
			float influence = pNode->GetInfluence();
			float relativeInfluence = abs(influence) / m_MaxAbsInfluence;

			if (influence < 0)
			{
				nodeColor = Elite::Color{
				Lerp(m_NeutralColor.r, m_NegativeColor.r, relativeInfluence),
				Lerp(m_NeutralColor.g, m_NegativeColor.g, relativeInfluence),
				Lerp(m_NeutralColor.b, m_NegativeColor.b, relativeInfluence)
				};
			}
			else
			{
				nodeColor = Elite::Color{
				Lerp(m_NeutralColor.r, m_PositiveColor.r, relativeInfluence),
				Lerp(m_NeutralColor.g, m_PositiveColor.g, relativeInfluence),
				Lerp(m_NeutralColor.b, m_PositiveColor.b, relativeInfluence)
				};
			}

			pNode->SetColor(nodeColor);
		}
	}

	template<class T_GraphType>
	inline void InfluenceMap<T_GraphType>::OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged)
	{
		InitializeBuffer();
	}
}