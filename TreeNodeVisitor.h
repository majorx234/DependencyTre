#pragma once
#include "TreeVertex.h"
#include <unordered_set>

namespace nsSandBox{
//forward decl
template <typename VertexIndex, typename EdgeData>
class Vertex;
//pulled out of class to avoid per-template instantiation
enum class VisitorOptions : int
{
	NONE,
	DETECT_LOOPS,
};

decltype(auto) operator&(VisitorOptions lhs, VisitorOptions rhs)
{
	using T = std::underlying_type_t<VisitorOptions>;
	return (static_cast<T>(lhs) & static_cast<T>(rhs));
}

enum class VisitorDirection
{
	ADJACENT,
	INCIDENT,
};

/**
* \brief	A tree node visitor.
* \author	smesserschmidt
* \date	3/9/2017
*/
template <typename VertexIndex, typename EdgeData>
class TreeNodeVisitor
{
public:
	TreeNodeVisitor(VisitorOptions options= VisitorOptions::NONE, VisitorDirection direction = VisitorDirection::ADJACENT)
		:mOptions(options)
		,mDirection(direction)
	{}

	virtual void apply(Vertex<VertexIndex, EdgeData>& v);
	void traverse(Vertex<VertexIndex, EdgeData>& v);

	VisitorDirection				mDirection	= VisitorDirection::ADJACENT;
	VisitorOptions					mOptions	= VisitorOptions::NONE;
	std::unordered_set<VertexIndex> mVisited;
};



template <typename VertexIndex, typename EdgeData>
void nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>::apply(Vertex<VertexIndex, EdgeData>& v)
{
	traverse(v);
}

template <typename VertexIndex, typename EdgeData>
void nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>::traverse(Vertex<VertexIndex, EdgeData>& v)
{
	if (VisitorOptions::DETECT_LOOPS & mOptions)
	{
		if (mVisited.count(v.getIndex()))
		{
			return;
		}
		mVisited.insert(v.getIndex());
	}
	v.traverse(*this);
}

}