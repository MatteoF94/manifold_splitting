//
// Created by matteo on 23/07/18.
//

#include <iostream>
#include <stack>
#include "KLabelPartitioner.h"

KLabelPartitioner::KLabelPartitioner(Graph graph) {
    mGraph = graph;

    // Initialize the vertex-label map so that the vertices have no label
    boost::graph_traits<Graph>::vertex_iterator vb,ve;
    for(boost::tie(vb,ve) = vertices(mGraph); vb != ve; ++vb) {
        mVertexLabelMap.insert({*vb,-1});
        mVertexGroupMap.insert({*vb,-1});
    }

    mMaxGroup = 0;
}

KLabelPartitioner::~KLabelPartitioner() {

}

void KLabelPartitioner::assignLabels(std::vector<int> labels) {
    boost::graph_traits<Graph>::vertex_iterator vb,ve;
    for(boost::tie(vb,ve) = vertices(mGraph); vb != ve; ++vb) {
        mVertexLabelMap[*vb] = labels.at(*vb);
    }
}

void KLabelPartitioner::clusterRecursively() {
    boost::graph_traits<Graph>::vertex_descriptor firstVertex = *(mGraph.vertex_set().begin());
    mVertexGroupMap[firstVertex] = mMaxGroup;
    boost::add_vertex(mReducedGraph);
    partitionRecursively(firstVertex);
    boost::print_graph(mReducedGraph);
}

// TODO should i pass the descriptor or the iterator?
void KLabelPartitioner::partitionRecursively(boost::graph_traits<Graph>::vertex_descriptor vd) {

    // Find the adjacency list in the graph
    std::list<boost::graph_traits<Graph>::vertex_descriptor> nodes_priority, nodes_no_priority;

    boost::graph_traits<Graph>::adjacency_iterator ab,ae;
    for(boost::tie(ab,ae) = boost::adjacent_vertices(vd,mGraph); ab != ae; ++ab) {

        if(mVertexGroupMap[*ab] == -1) {  // The vertex is still not explored
            if(mVertexLabelMap[*ab] == mVertexLabelMap[vd]) {
                nodes_priority.push_back(*ab);
                mVertexGroupMap[*ab] = mVertexGroupMap[vd];
            }
            else {
                nodes_no_priority.push_back(*ab);
            }
        }
    }

    std::list<boost::graph_traits<Graph>::vertex_descriptor>::iterator nodes_priority_it;
    for (nodes_priority_it = nodes_priority.begin(); nodes_priority_it != nodes_priority.end(); ++nodes_priority_it)
        partitionRecursively(*nodes_priority_it);

    std::list<boost::graph_traits<Graph>::vertex_descriptor>::iterator nodes_no_priority_it;
    for (nodes_no_priority_it = nodes_no_priority.begin(); nodes_no_priority_it != nodes_no_priority.end(); ++nodes_no_priority_it) {
        if(mVertexGroupMap[*nodes_no_priority_it] == -1) {
            ++mMaxGroup;
            boost::add_vertex(mReducedGraph);
            mVertexGroupMap[*nodes_no_priority_it] = mMaxGroup;
            partitionRecursively(*nodes_no_priority_it);
        }
        boost::add_edge(mVertexGroupMap[vd],mVertexGroupMap[*nodes_no_priority_it],mReducedGraph);
    }

    return;
}

void KLabelPartitioner::clusterCyclically() {

    struct SnapShotStruct {
        boost::graph_traits<Graph>::vertex_descriptor descriptor;
        std::list<boost::graph_traits<Graph>::vertex_descriptor> nodes_priority, nodes_no_priority;
        std::list<boost::graph_traits<Graph>::vertex_descriptor>::iterator nodes_no_priority_it;
        int stage;
    };

    std::list<SnapShotStruct> snapshot_list;

    SnapShotStruct current_snapshot;
    current_snapshot.descriptor = *(mGraph.vertex_set().begin());
    mVertexGroupMap[current_snapshot.descriptor] = mMaxGroup;
    boost::graph_traits<Graph>::adjacency_iterator ab,ae;
    current_snapshot.stage = 0;
    snapshot_list.push_back(current_snapshot);
    boost::add_vertex(mReducedGraph);

    std::list<boost::graph_traits<Graph>::vertex_descriptor>::iterator nodes_priority_it;
    std::list<SnapShotStruct>::iterator snap_it = snapshot_list.begin();

    while(!snapshot_list.empty()) {
        //std::cout << "Current id: " << snap_it->descriptor << " and stage: " << snap_it->stage << std::endl;
        switch (snap_it->stage) {
            case 0:

                for(boost::tie(ab,ae) = boost::adjacent_vertices(snap_it->descriptor,mGraph); ab != ae; ++ab) {

                    if(mVertexGroupMap[*ab] == -1) {  // The vertex is still not explored
                        if(mVertexLabelMap[*ab] == mVertexLabelMap[snap_it->descriptor]) {
                            snap_it->nodes_priority.push_back(*ab);
                            mVertexGroupMap[*ab] = mVertexGroupMap[snap_it->descriptor];
                        }
                        else {
                            snap_it->nodes_no_priority.push_back(*ab);
                        }
                    }
                }
                if (snap_it->nodes_no_priority.size() != 0)
                    snap_it->nodes_no_priority_it = snap_it->nodes_no_priority.begin();


                if (snap_it->nodes_priority.size() != 0) {
                    for (nodes_priority_it = snap_it->nodes_priority.begin(); nodes_priority_it != snap_it->nodes_priority.end(); ++nodes_priority_it) {
                        SnapShotStruct new_snapshot;
                        new_snapshot.stage = 0;
                        new_snapshot.descriptor = *nodes_priority_it;
                        snapshot_list.push_back(new_snapshot);
                    }
                }

                snap_it->stage = 1;
                if(std::next(snap_it,1) != snapshot_list.end())
                    snap_it++;

                break;
            case 1:

                if (snap_it->nodes_no_priority.size() != 0) {
                    if(snap_it->nodes_no_priority_it == snap_it->nodes_no_priority.end()) {
                        snap_it->stage = 2;
                        break;
                    }

                    if (mVertexGroupMap[*snap_it->nodes_no_priority_it] == -1) {
                        ++mMaxGroup;
                        boost::add_vertex(mReducedGraph);
                        mVertexGroupMap[*snap_it->nodes_no_priority_it] = mMaxGroup;
                        SnapShotStruct new_snapshot;
                        new_snapshot.stage = 0;
                        new_snapshot.descriptor = *snap_it->nodes_no_priority_it;
                        snapshot_list.push_back(new_snapshot);
                        boost::add_edge(mVertexGroupMap[snap_it->descriptor],mVertexGroupMap[*snap_it->nodes_no_priority_it],mReducedGraph);
                        snap_it->nodes_no_priority_it++;
                        snap_it++;
                    } else {
                        boost::add_edge(mVertexGroupMap[snap_it->descriptor],mVertexGroupMap[*snap_it->nodes_no_priority_it],mReducedGraph);
                        snap_it->nodes_no_priority_it++;
                    }

                } else {
                    snap_it->stage = 2;
                }

                break;

            case 2:
                if(snap_it->descriptor == 0)
                    std::cout << "OI MATE" << std::endl;

                if(snap_it != snapshot_list.begin())
                    snap_it--;
                snapshot_list.pop_back();

                break;
        }
    }

    //boost::print_graph(mReducedGraph);
}

std::vector<VertexInfo> KLabelPartitioner::createClusters() {
    int num_groups = static_cast<int>(boost::num_vertices(mReducedGraph));

    for (int i = 0; i < num_groups; i++) {
        VertexInfo tmp_node = VertexInfo();
        mReducedGraphNodes.push_back(tmp_node);
    }

    boost::graph_traits<Graph>::vertex_iterator vb,ve;
    for(boost::tie(vb,ve) = vertices(mGraph); vb != ve; ++vb) {
        int curr_group = mVertexGroupMap[*vb];
        if(curr_group == -1) curr_group = 0;
        mReducedGraphNodes.at(curr_group).nodes.push_back(*vb);
    }

    for (int i = 0; i < num_groups; i++) {
        int curr_label = mVertexLabelMap[mReducedGraphNodes.at(i).nodes.at(0)];
        mReducedGraphNodes.at(i).label = curr_label;

        boost::graph_traits<Graph>::adjacency_iterator ab,ae;
        for(boost::tie(ab,ae) = boost::adjacent_vertices(i,mReducedGraph); ab != ae; ++ab) {
            mReducedGraphNodes.at(i).adjacent_groups.push_back(*ab);
        }
    }

    return mReducedGraphNodes;
}

void KLabelPartitioner::printClusters() {
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor,int>::iterator map_it;
    for(map_it = mVertexGroupMap.begin(); map_it != mVertexGroupMap.end(); ++map_it) {
        std::cout << "Vertex " << map_it->first << " in group " << map_it->second << std::endl;
    }
}