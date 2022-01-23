#pragma once
#include "../EIGraph.h"
//namespace Elite
//{
//	template <class T_NodeType, class T_ConnectionType>
//	class IGraph;
//}

template<class T_NodeType, class T_ConnectionType>
using GraphClass = Elite::IGraph<T_NodeType, T_ConnectionType>;

template <class T_NodeType, class T_ConnectionType>
class GraphColouring
{
public:
	GraphColouring(Elite::IGraph<T_NodeType, T_ConnectionType>* pGraph);
	void UpdateColours();

	/*static const Elite::Color AquaMenthe;
	static const Elite::Color ProtonPurple;
	static const Elite::Color OrangeRed;
	static const Elite::Color OfficeGreen;
	static const Elite::Color Blue;
	static const Elite::Color BlushPink;
	static const Elite::Color CosmicLatte;
	static const Elite::Color Green;
	static const Elite::Color Red;
	static const Elite::Color Yellow;
	static const Elite::Color Grey;
	static const Elite::Color Black;*/

private:
	std::vector<Elite::Color> m_Colours;
	Elite::IGraph<T_NodeType, T_ConnectionType>* m_pGraph;

	//Private helper functions
	void InitialiseColourVector();
};

//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::AquaMenthe = { 0.498f, 1.f, 0.831f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::ProtonPurple = { 0.541f, 0.168f, 0.886f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::OrangeRed = { 1.f, 0.271f, 0.f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::OfficeGreen = { 0.f, 0.501f, 0.f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::Blue = { 0.f, 0.f, 1.f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::BlushPink = { 1.f, 0.752f, 0.796f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::CosmicLatte = { 1.f, 0.976f, 0.890f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::Green = { 0.f, 1.f, 0.f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::Red = { 1.f, 0.f, 0.f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::Yellow = { 1.f, 1.f, 0.f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::Grey = { 0.73f, 0.73f, 0.73f };
//template<class T_NodeType, class T_ConnectionType>
//const Elite::Color GraphColouring<T_NodeType, T_ConnectionType>::Black = { 0.f, 0.f, 0.f};

template<class T_NodeType, class T_ConnectionType>
inline GraphColouring<T_NodeType, T_ConnectionType>::GraphColouring(Elite::IGraph<T_NodeType, T_ConnectionType>* pGraph)
	: m_pGraph{ pGraph }
{
	//std::cout << "initialised!";
	InitialiseColourVector();

	std::vector<T_NodeType*> pNodes = m_pGraph->GetAllNodes();
	std::for_each(pNodes.begin(), pNodes.end(), [](T_NodeType* pNode) { pNode->SetColor(Elite::Color::Black); });
}

template<class T_NodeType, class T_ConnectionType>
inline void GraphColouring<T_NodeType, T_ConnectionType>::UpdateColours()
{
	//std::cout << "This class works";

	//bool empty = m_pGraph->IsEmpty();
	//if (empty)
	//	std::cout << empty;
	//else
	//	std::cout << empty;

	std::vector<T_NodeType*> pNodes = m_pGraph->GetAllActiveNodes();
	std::list<T_ConnectionType*> pConnections{};
	for (T_NodeType* pNode : pNodes)
	{
		int32_t i{ -1 };
		Elite::Color selectedColour = Elite::Color::Black;
		bool succes = true;
		do
		{
			++i;
			succes = true;
			if (i > m_Colours.size())
				selectedColour = Elite::Color::Black;
			else
				selectedColour = m_Colours[i]; //select colour
			pConnections = m_pGraph->GetNodeConnections(pNode);
			for (T_ConnectionType* pConnection : pConnections)
			{
				int neighbour = pConnection->GetTo();
				if (m_pGraph->GetNode(neighbour)->GetColor() == selectedColour)
				{
					succes = false;
					break;
				}
			}
		} while (!succes);

		pNode->SetColor(selectedColour);
	}
}


template<class T_NodeType, class T_ConnectionType>
inline void GraphColouring<T_NodeType, T_ConnectionType>::InitialiseColourVector()
{
	m_Colours.push_back(Elite::Color::AquaMenthe);
	m_Colours.push_back(Elite::Color::ProtonPurple);
	m_Colours.push_back(Elite::Color::OrangeRed);
	m_Colours.push_back(Elite::Color::OfficeGreen);
	m_Colours.push_back(Elite::Color::Blue);
	m_Colours.push_back(Elite::Color::BlushPink);
	m_Colours.push_back(Elite::Color::CosmicLatte);
	m_Colours.push_back(Elite::Color::Green);
	m_Colours.push_back(Elite::Color::Red);
	m_Colours.push_back(Elite::Color::Yellow);
	m_Colours.push_back(Elite::Color::Grey);
}