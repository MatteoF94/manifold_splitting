//
// Created by matteo on 10/10/18.
//

#include "KSizeClustering.h"

std::vector<VertexInfo> KSizeClustering::reduceGraph(std::vector<VertexInfo> vertices){

    if(vertices.size() <= 2500) {
        /*for(int i = 0; i < vertices.size(); ++i)
            vertices.at(i).label = i;*/
        return vertices;
    }

    std::vector<std::vector<int>> new_vertices;
    std::unordered_map<int,bool> assignment_map;
    std::unordered_map<int,int> group_map;
    for (int i = 0; i < vertices.size(); ++i) {
        assignment_map.insert({i,false});
    }

    int a=0;
    int b=0;
    for (int i = 0; i < vertices.size(); ++i) {
        VertexInfo curr_vertex = vertices.at(static_cast<unsigned long>(i));
        group_map.insert({i,-1});

        if(curr_vertex.nodes.size() < 50000) {
            if(!assignment_map.at(i)) {
                std::vector<int> neighbours;
                std::vector<int> neighbours_size;

                for(auto &vertex : curr_vertex.adjacent_groups) {
                    if(!assignment_map.at(vertex)) {
                        neighbours.push_back(vertex);
                        neighbours_size.push_back(static_cast<int &&>(vertices.at(static_cast<unsigned long>(vertex)).nodes.size()));
                    }
                }

                if(!neighbours.empty()) {
                    int min_size_pos = static_cast<int>(std::min_element(neighbours_size.begin(), neighbours_size.end()) - neighbours_size.begin());
                    int min_vertex = neighbours.at(static_cast<unsigned long>(min_size_pos));

                    assignment_map[min_vertex] = true;
                    assignment_map[i] = true;

                    std::vector<int> new_vertex = {i, min_vertex};
                    new_vertices.push_back(new_vertex);
                    a++;

                } else {
                    std::vector<int> new_vertex = {i};
                    new_vertices.push_back(new_vertex);
                    b++;
                }
            }
        }
    }

    for (int i = 0; i < vertices.size(); ++i) {
        VertexInfo curr_vertex = vertices.at(static_cast<unsigned long>(i));
        if(curr_vertex.nodes.size() >= 50000) {
            if (!assignment_map.at(i)) {
                std::vector<int> new_vertex = {i};
                new_vertices.push_back(new_vertex);
            }
        }
    }

    // Merging phase
    std::vector<VertexInfo> infos;
    std::vector<int> sizes;

    for(int i = 0; i < new_vertices.size(); ++i) {
        std::vector<int> new_vertex = new_vertices.at(i);
        VertexInfo vertex_info;
        for(auto &node : new_vertex) {
            vertex_info.nodes.insert(vertex_info.nodes.end(), vertices.at(node).nodes.begin(),
                                     vertices.at(node).nodes.end());

            group_map[node] = i;
            sizes.push_back(vertex_info.nodes.size());
        }

        int tmp;
        if(new_vertex.size() > 1)
            tmp = vertices.at(new_vertex.at(0)).nodes.size() > vertices.at(new_vertex.at(1)).nodes.size() ? new_vertex.at(0) : new_vertex.at(1);
        else
            tmp = new_vertex.at(0);
        vertex_info.label = vertices.at(tmp).label;

        infos.push_back(vertex_info);
    }

    int ass = infos.size();

    for(int i = 0; i < infos.size(); ++i) {
        VertexInfo* vertex_info = &infos.at(i);
        std::unordered_set<int> group_set;
        int curr_group = group_map[new_vertices.at(i).at(0)];


        for(auto &node : new_vertices.at(i)) {
            for(auto &neighbour: vertices.at(node).adjacent_groups) {
                int node_group = group_map[neighbour];
                if (node_group != curr_group)
                    group_set.insert(node_group);
            }
        }

        vertex_info->adjacent_groups.insert(vertex_info->adjacent_groups.end(),group_set.begin(),group_set.end());
    }

    if(infos.size() == vertices.size())
        return infos;

    return reduceGraph(infos);
}