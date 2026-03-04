
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <ranges>

#include "Graphs.hpp"


namespace Graphs
{

    DijkstraResult dijkstra
    ( 
        VertexID start_node,  
        const DijkstraAdjList& adj 
    )

    {
        std::unordered_map<VertexID, Distance> distances;
        std::unordered_map<VertexID, VertexID> parents;

        auto keys_view = adj | std::views::keys;
        auto n = keys_view.size();

        distances.reserve(n);
        parents.reserve(n);

        VertexID default_vertex_id{-1, -1};

        for ( const auto& key : keys_view )
        { 
            distances.emplace(key, 1e9);
            parents.emplace(key, default_vertex_id); 
        }

        std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<NodeDist>> pq;

        distances[start_node] = 0;
        pq.push( { distances.at(start_node), start_node } );

        while ( !pq.empty() )
        {
            auto [ d, u ] = pq.top();
            pq.pop();

            auto d_u = distances.at(u);

            if ( d > d_u )
            { continue; }

            for ( const auto& edge : adj.at(u) )
            {
                auto [ v, d_u_v ] = edge;
                
                double new_d = d_u + d_u_v;

                auto& d_v = distances.at(v);

                if ( new_d < d_v ) 
                {
                    d_v = new_d;
                    parents[v] = u;
                    pq.push({d_v, v});
                }
            }
        }

        return { distances, parents };
    }  
    
    Collection get_path
    ( 
        VertexID start_node, 
        VertexID target, 
        const std::unordered_map<VertexID, VertexID>& parents 
    )
    
    {
        Collection path;
        VertexID default_vertex_id{-1, -1};

        if ( parents.at(target) == default_vertex_id && target != start_node ) 
        { return path; }

        for ( VertexID v = target; v != default_vertex_id; v = parents.at(v) )
        { path.push_back(v); }

        std::reverse( path.begin(), path.end() );
        return path;
    }

    //----------------------------------------------------
}