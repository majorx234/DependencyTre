#pragma once
#include "TreeNodeVisitor.h"

#include <memory>
#include <vector>
#include <string>

namespace nsSandBox{
//forward decl
template <typename VertexIndex, typename EdgeData>
class Vertex;

template <typename VertexIndex, typename EdgeData>
struct Edge
{
	typedef Vertex<VertexIndex, EdgeData> VertexType;

	Edge(const std::shared_ptr<VertexType>& end_point, const EdgeData& edge_data)
		:mEndPoint(end_point)
		, mEdgeData(edge_data)
	{
	}
	VertexIndex getEndPointIndex() const
	{
		if (mEndPoint.expired())
		{
			throw std::logic_error("Invalid Edge endpoint");
		}
		return mEndPoint.lock()->getIndex();
	}
	std::weak_ptr<VertexType> mEndPoint;
	EdgeData				  mEdgeData; ///< a weight etc. 
};

//////////////////////////////////////////////////////////////////////////
template <typename VertexIndex, typename EdgeData>
class Vertex : public std::enable_shared_from_this<Vertex<VertexIndex, EdgeData>>
{
public:
	typedef Vertex<VertexIndex, EdgeData>	VertexType;
	typedef Edge<VertexIndex, EdgeData>		EdgeType;
	typedef std::vector<EdgeType>			EdgeList;

	template <typename VertexIndex, typename EdgeData>
	friend class Graph;
public:
	Vertex(VertexIndex index)
		:mIndex(index)
	{}

	const VertexIndex& getIndex() const
	{
		return mIndex;
	}

	void addAdjacentEdge(const std::shared_ptr<Vertex>& other, const EdgeData& data)
	{
		auto a = EdgeType(other, data);
		mAdjacentVertices.emplace_back(std::move(a));
		//#TODO: check for unique list
		other->mIncidentVertices.emplace_back(shared_from_this(), data);
	}
	bool removeAdjacentEdge(const VertexIndex& other )
	{
		auto is_matching_node = [&other](const auto& edge) {return other == edge.getEndPointIndex(); };
		auto iter = std::find_if(std::begin(mAdjacentVertices), std::end(mAdjacentVertices), is_matching_node);
		if (iter != mAdjacentVertices.end())
		{
			auto edge = (*iter);
			mAdjacentVertices.erase(iter);
			edge.mEndPoint.lock()->removeIncidentEdge(getIndex());
			return true;
		}
	
		
		return false;
	}
	bool removeIncidentEdge(const VertexIndex& other)
	{
		auto is_matching_node = [&other](const auto& edge) {return other == edge.getEndPointIndex(); };
		auto iter = std::find_if(std::begin(mIncidentVertices), std::end(mIncidentVertices), is_matching_node);
		if (iter != mIncidentVertices.end())
		{
			//copy edge
			auto edge = (*iter);
			mIncidentVertices.erase(iter);
			edge.mEndPoint.lock()->removeAdjacentEdge(getIndex());
			
			return true;
		}


		return false;
	}

	decltype(auto) adjacent_begin() const
	{
		return std::begin(mAdjacentVertices);
	}
	decltype(auto) adjacent_end() const
	{
		return std::end(mAdjacentVertices);
	}
	decltype(auto) incident_begin() const
	{
		return std::begin(mIncidentVertices);
	}
	decltype(auto) incident_end() const
	{
		return std::end(mIncidentVertices);
	}
	size_t sizeAdjacent() const
	{
		return mAdjacentVertices.size();
	}
	size_t sizeIncident() const
	{
		return mIncidentVertices.size();
	}
	//////////////////////////////////////////////////////////////////////////
	// VISITOR pattern
	void accept(TreeNodeVisitor<VertexIndex, EdgeData>& visitor)
	{
		visitor.apply(*this);
	}

	void traverse(TreeNodeVisitor<VertexIndex, EdgeData>& visitor)
	{
		
		if (VisitorDirection::ADJACENT == visitor.mDirection)
		{
			for (auto& v : mAdjacentVertices)
			{
				auto shared_vertex = v.mEndPoint.lock();
				if (shared_vertex)
				{
					shared_vertex->accept(visitor);
				}
			}
		}
		else
		{
			for (auto& v : mIncidentVertices)
			{
				auto shared_vertex = v.mEndPoint.lock();
				if (shared_vertex)
				{
					shared_vertex->accept(visitor);
				}
			}
		}
	}
protected:

	//simple edge

	std::vector<EdgeType>	mAdjacentVertices; //list of adjacent vertices
	std::vector<EdgeType>	mIncidentVertices; //list of incident vertices
	VertexIndex				mIndex;	//< any sortable index should do

};
//////////////////////////////////////////////////////////////////////////
} //namespace
