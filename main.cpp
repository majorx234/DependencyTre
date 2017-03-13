#include <iostream>

#include "TreeVertex.h"
#include "TreeVisitors.h"

#include <iterator>
#include <algorithm>
#include <deque>
//#TODO: majorx234 @github 
//for now a classic tree
namespace nsSandBox
{
//the graph is only a helper class to hold a list of all vertices and possibly some adjacency/incidence information
// 

template <class VertexIndex = std::string, typename EdgeData = float>
class Graph
{
public:
	typedef Vertex<VertexIndex, EdgeData> VertexType;

	void addVertex(const std::shared_ptr<VertexType>& vertex)
	{
		auto is_equal_name = [&vertex](const auto& v) {return v->mIndex == vertex->mIndex; };
		auto iter = std::find_if(std::begin(mVertices), std::end(mVertices), is_equal_name);
		if (iter == mVertices.end())
		{
			mVertices.push_back(vertex);
		}
		else
		{
			throw std::logic_error("Vertex already in graph");
			return;
		}
	}

	void hasVertex(const VertexIndex&  v1 ) const
	{
		return nullptr != getVertex(v1) ;
	}

	void addPreEdge(const VertexIndex&  v1, const VertexIndex& successor, const EdgeData& edge_data = EdgeData())
	{
		auto& vertex_1 = getVertex(v1);
		auto& vertex_2 = getVertex(successor);
		vertex_1->addAdjacentEdge(vertex_2, edge_data);
	}

	void addPostEdge(const VertexIndex&  v1, const VertexIndex& predecessor, const EdgeData& edge_data = EdgeData())
	{
		auto& vertex_1 = getVertex(v1);
		auto& vertex_2 = getVertex(predecessor);
		vertex_2->addAdjacentEdge(vertex_1, edge_data);
	}
	std::shared_ptr<VertexType> getVertex(const std::string& name) const
	{
		auto is_equal_name = [&name](const auto& v) {return v->mIndex == name; };
		auto iter = std::find_if(std::begin(mVertices), std::end(mVertices), is_equal_name);
		if (iter != mVertices.end())
		{
			return *iter;
		}
		throw std::out_of_range("No such vertex");
	}
	std::vector<VertexIndex> getVertices() const
	{
		std::vector<VertexIndex> vert_indices;
		std::transform(std::begin(mVertices), std::end(mVertices), std::back_inserter(vert_indices), [](const auto&  vertex) {return vertex->getIndex(); });
		return vert_indices;
	}

protected:
	
	std::vector<std::shared_ptr<VertexType>> mVertices;
};



template <typename VertexIndex, typename EdgeData>
std::vector<VertexIndex> topological_sort(Graph<VertexIndex, EdgeData> graph)
{
	//basically Kahn's algorithm
	typedef Graph<VertexIndex, EdgeData> GraphT;
	
	std::deque<std::shared_ptr<GraphT::VertexType>> sink_vertices; ///< all without incoming edges (S)
	std::vector<std::shared_ptr<GraphT::VertexType>> list_vertices; ///Result list (L)
	auto vertices = graph.getVertices();
	//will move all with no incident vertices to sink_vertices
	auto copy_sinks = [&sink_vertices,&graph](const auto& vertex_index)
	{
		auto v = graph.getVertex(vertex_index);
		size_t s = std::distance(v->incident_begin(), v->incident_end());
		if (0 == s)
		{
			sink_vertices.push_back(v);
		};
	};

	///find all vertices with no incident vertices (sinks)
	std::for_each(std::begin(vertices), std::end(vertices), copy_sinks);
	while (!sink_vertices.empty())
	{ 
		//remove a node N from S
		//and put it at tail of 
		list_vertices.emplace_back(std::move(sink_vertices.back()));
		auto& n = list_vertices.back();
		sink_vertices.pop_back();
		std::vector<GraphT::VertexType::EdgeType> adjacent_edges(n->adjacent_begin(), n->adjacent_end());
		
		for (const auto& iter : adjacent_edges)
		{
			//remove edge from n to m
			auto shared_m = iter.mEndPoint.lock();
			if (shared_m)
			{
				shared_m->removeIncidentEdge(n->getIndex());
			}
			//no further incident -> additonal sink
			if (!shared_m->sizeIncident())
			{
				sink_vertices.push_back(shared_m);
			}
		}
	}
	//count remaining edges
	for (const auto& vertex : graph.getVertices())
	{
		if (graph.getVertex(vertex)->sizeIncident()
			||
			graph.getVertex(vertex)->sizeAdjacent()
			)
		{
			//#TODO: report node which are cyclic
			throw std::logic_error("Graph has dependency cycles");
		}
	}
	
	std::vector<VertexIndex> result; 
	std::transform(std::begin(list_vertices), std::end(list_vertices), std::back_inserter(result), [](const auto& vertex) {return vertex->getIndex(); });
	return result;
}
}
void main()
{
	try 
	{

		using Vertex = nsSandBox::Vertex<std::string, float>;
		nsSandBox::Graph<std::string, float> graph;

		///https://de.wikipedia.org/wiki/Topologische_Sortierung
		graph.addVertex(std::make_shared<Vertex>("UNTERHOSE"));
		graph.addVertex(std::make_shared<Vertex>("HOSE"));
		graph.addVertex(std::make_shared<Vertex>("MANTEL"));
		graph.addVertex(std::make_shared<Vertex>("SCHUHE"));
		graph.addVertex(std::make_shared<Vertex>("SOCKEN"));
		graph.addVertex(std::make_shared<Vertex>("UNTERHEMD"));
		graph.addVertex(std::make_shared<Vertex>("PULLOVER"));

		graph.addPreEdge("UNTERHOSE", "HOSE"); //adds "UNTERHOSE" as predecessor (aka incident edge to "HOSE"
		graph.addPreEdge("HOSE", "MANTEL");
		graph.addPreEdge("UNTERHEMD", "PULLOVER");
		graph.addPreEdge("PULLOVER", "MANTEL");
		graph.addPreEdge("SOCKEN", "SCHUHE");
		graph.addPreEdge("HOSE", "SCHUHE");

		for(auto& v : graph.getVertices())
		{
			DiscoverTreeVisitor<std::string, float> vistor;

			graph.getVertex(v)->accept(vistor);
			std::cout << "found tree of size: " << vistor.mVisitedVertices.size() << " with starting node " << v <<  std::endl;
		}



// 		FindSourceNodesVisitor<std::string, float> source_vistor;
// 		graph.getVertex("A")->accept(source_vistor);

		nsSandBox::topological_sort(graph);
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	system("pause");
	//graph.
	
}
