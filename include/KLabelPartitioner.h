//
// Created by matteo on 23/07/18.
//

#ifndef MANIFOLD_SPLITTING_KLABELPARTITIONER_H
#define MANIFOLD_SPLITTING_KLABELPARTITIONER_H


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

typedef boost::adjacency_list<boost::setS,boost::vecS,boost::undirectedS> Graph;

struct VertexInfo {
    int label;
    std::vector<int> nodes;
    std::vector<int> adjacent_groups;
};

class KLabelPartitioner {
public:
    KLabelPartitioner(Graph graph);
    virtual ~KLabelPartitioner();

    void assignLabels(std::vector<int> labels);
    void clusterRecursively();
    void clusterCyclically();
    std::vector<VertexInfo> createClusters();
    void printClusters();

private:
    Graph mGraph;
    Graph mReducedGraph;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor,int> mVertexLabelMap;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor,int> mVertexGroupMap;
    int mMaxGroup;
    std::vector<VertexInfo> mReducedGraphNodes;

    void partitionRecursively(boost::graph_traits<Graph>::vertex_descriptor vd);
};


#endif //MANIFOLD_SPLITTING_KLABELPARTITIONER_H
