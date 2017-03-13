#pragma once
#include "TreeNodeVisitor.h"
#include <set>

//////////////////////////////////////////////////////////////////////////
template <typename VertexIndex, typename EdgeData>
class PrintVisitor : public nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>
{
	typedef nsSandBox::Vertex<VertexIndex, EdgeData> VertexType;

public:
	PrintVisitor(nsSandBox::VisitorDirection direction =nsSandBox::VisitorDirection::ADJACENT )
		:nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>(nsSandBox::VisitorOptions::DETECT_LOOPS, direction)
	{}

	virtual void apply(VertexType & v) override
	{
		std::cout << v.getIndex() << std::endl;
		traverse(v);
	}

};

//////////////////////////////////////////////////////////////////////////
template <typename VertexIndex, typename EdgeData>
class FindSourceNodesVisitor : public nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>
{
	typedef nsSandBox::Vertex<VertexIndex, EdgeData> VertexType;

public:
	FindSourceNodesVisitor()
		:nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>(nsSandBox::VisitorOptions::DETECT_LOOPS)
	{}

	virtual void apply(VertexType & v) override
	{
		size_t num_incident = std::distance(v.incident_begin(), v.incident_end());
		if (0 == num_incident)
		{
			mSourceVertexIndices.push_back(v.getIndex());
		}
		traverse(v);
	}

	std::vector<VertexIndex> mSourceVertexIndices;

};

//////////////////////////////////////////////////////////////////////////
template <typename VertexIndex, typename EdgeData>
class DiscoverTreeVisitor : public nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>
{
	typedef nsSandBox::Vertex<VertexIndex, EdgeData> VertexType;

public:
	DiscoverTreeVisitor()
		:nsSandBox::TreeNodeVisitor<VertexIndex, EdgeData>(nsSandBox::VisitorOptions::NONE)
	{}

	virtual void apply(VertexType & v) override
	{
		if (mVisitedVertices.count(v.getIndex()))
		{
			return;
		}
		mVisitedVertices.insert(v.getIndex());
		//recursively assemble a list following all adjacent vertices
		mDirection = nsSandBox::VisitorDirection::ADJACENT;
		traverse(v);

		mDirection = nsSandBox::VisitorDirection::INCIDENT;
		traverse(v);

	}

	std::set<VertexIndex> mVisitedVertices;

};


