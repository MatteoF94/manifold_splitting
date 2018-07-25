//
// Created by matteo on 23/07/18.
//

#ifndef MANIFOLD_SPLITTING_KLABELPARTITIONER_H
#define MANIFOLD_SPLITTING_KLABELPARTITIONER_H


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

typedef boost::adjacency_list<boost::setS,boost::vecS,boost::undirectedS> Graph;

class KLabelPartitioner {
public:
    KLabelPartitioner(Graph graph);
    virtual ~KLabelPartitioner();

    void assignLabels(std::vector<int> labels);
    void partitionRecursively();
    void partitionCyclically ();
    void printGroups();

private:
    Graph mGraph;
    Graph mReducedGraph;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor,int> mVertexLabelMap;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor,int> mVertexGroupMap;
    int mMaxGroup;

    void partitionRecursively(boost::graph_traits<Graph>::vertex_descriptor vd);
};


#endif //MANIFOLD_SPLITTING_KLABELPARTITIONER_H
