//
// Created by matteo on 17/11/18.
//

#include <fstream>
#include "RegularGraphsGen.h"

RegularGraphsGen::RegularGraphsGen(unsigned int n) {
    n_ = n;
}

Graph RegularGraphsGen::createKRegGraph(int k) {
    if(n_%2 == 1 && k%2 == 1) {
        std::cout << "**ERROR** :: n is odd, k should be even" << std::endl;
    }

    if(n_%2 == 0)
        return createGraphNEven(k);
    else
        return createGraphNOdd(k);
}

Graph RegularGraphsGen::createGraphNEven(int k) {
    Graph reg_graph(n_);
    std::vector<unsigned int> jumps;

    int count_numbers;
    if(k%2 == 0) {
        count_numbers = k / 2;
        for(unsigned int i = 1; i <= count_numbers; ++i)
            jumps.emplace_back(i);
    }
    else {
        count_numbers = (k - 1) / 2 + 1;
        for(unsigned int i = 1; i <= count_numbers; ++i)
            jumps.emplace_back(i);
    }

    for(unsigned int i = 0; i < n_; ++i) {
        for(unsigned int &jump : jumps) {
            unsigned int source = i;
            unsigned int target = (i + jump)%n_;
            boost::add_edge(source,target,reg_graph);
        }
    }

    return reg_graph;
}

Graph RegularGraphsGen::createGraphNOdd(int k) {
    Graph reg_graph(n_);
    std::vector<unsigned int> jumps;

    for(int i = 1; i <= k; ++i)
        if(i%2 == 1)
            jumps.emplace_back(i);

    for(unsigned int i = 0; i < n_; ++i) {
        for(unsigned int &jump : jumps) {
            unsigned int source = i;
            unsigned int target = (i + jump)%n_;
            boost::add_edge(source,target,reg_graph);
        }
    }

    return reg_graph;
}

void RegularGraphsGen::writeGraphMetis(Graph g, const std::string &filename){
    std::ofstream outfile(filename);
    outfile << boost::num_vertices(g) << " " << boost::num_edges(g) << std::endl;

    for(unsigned long i = 0; i < boost::num_vertices(g); ++i) {
        boost::graph_traits<Graph>::adjacency_iterator vb,ve;
        for(boost::tie(vb,ve) = boost::adjacent_vertices(i,g); vb != ve; ++vb) {
            outfile << *vb + 1 << " ";
        }
        outfile << std::endl;
    }
}