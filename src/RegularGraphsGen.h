//
// Created by matteo on 17/11/18.
//

#ifndef MANIFOLD_SPLITTING_REGULARGRAPHSGEN_H
#define MANIFOLD_SPLITTING_REGULARGRAPHSGEN_H

#include <types.h>

class RegularGraphsGen {
public:
    RegularGraphsGen(unsigned int n);
    Graph createKRegGraph(int k);

    void writeGraphMetis(Graph g, const std::string &filename);
private:
    Graph createGraphNEven(int k);
    Graph createGraphNOdd(int k);

    unsigned int n_;
};


#endif //MANIFOLD_SPLITTING_REGULARGRAPHSGEN_H
