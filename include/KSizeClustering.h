//
// Created by matteo on 10/10/18.
//

#ifndef MANIFOLD_SPLITTING_KSIZECLUSTERING_H
#define MANIFOLD_SPLITTING_KSIZECLUSTERING_H

#include <KLabelPartitioner.h>

class KSizeClustering {
public:
    std::vector<VertexInfo> reduceGraph(std::vector<VertexInfo> vertices);
    Graph reduceGraphWithMerging(Graph g);
private:
    int mThreshold;
};


#endif //MANIFOLD_SPLITTING_KSIZECLUSTERING_H
