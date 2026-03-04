

#ifndef GRAPHS_HPP_
#define GRAPHS_HPP_

#include <unordered_map>
#include <set>
#include <vector>
#include <list>
#include "Vector2D.hpp"

namespace Graphs
{
    using VertexID = Vector2D<int>;
    using Collection = std::vector<VertexID>;
    using Distance = double;
    using DijkstraAdjList = std::unordered_map< VertexID, std::vector<std::pair<VertexID, Distance>>>;
    using NodeDist = std::pair<Distance, VertexID>;

    struct DijkstraResult
    {
        std::unordered_map<VertexID, Distance> distances;
        std::unordered_map<VertexID, VertexID> parents;
    };

    DijkstraResult dijkstra
    ( 
        VertexID start_node, 
        const DijkstraAdjList& adj 
    );

    Collection get_path
    ( 
        VertexID start_node, 
        VertexID target, 
        const std::unordered_map<VertexID, VertexID>& parents 
    );
}



#endif