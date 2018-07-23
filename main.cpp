#include <iostream>
#include <KLabelPartitioner.h>
#include <boost/graph/graph_utility.hpp>
#include <stack>
#include "include/InputManager.h"
#include "include/GraphParser.h"

int main() {
    InputManager inputManager;

    inputManager.readMeshFromOff("../data/bunny.off");
    //inputManager.readMeshFromOff("./cube_quad.off");
    Graph g = inputManager.meshToGraphDual();

    /*Graph g(11);
    boost::add_edge(0,4,g);
    boost::add_edge(0,7,g);
    boost::add_edge(0,10,g);
    boost::add_edge(1,3,g);
    boost::add_edge(1,6,g);
    boost::add_edge(1,8,g);
    boost::add_edge(2,3,g);
    boost::add_edge(2,5,g);
    boost::add_edge(2,8,g);
    boost::add_edge(2,9,g);
    boost::add_edge(3,8,g);
    boost::add_edge(4,6,g);
    boost::add_edge(4,7,g);
    boost::add_edge(5,7,g);
    boost::add_edge(5,8,g);
    boost::add_edge(5,9,g);
    boost::add_edge(6,8,g);
    boost::add_edge(7,9,g);
    boost::add_edge(7,10,g);
    boost::add_edge(9,10,g);
    boost::print_graph(g);*/

    KLabelPartitioner k_label_partitioner(g);

    //std::vector<int> MIAO {1,1,2,3,3,2,3,1,3,1,1};
    //k_label_partitioner.assignLabels(MIAO);
    k_label_partitioner.partitionRecursively();
    //k_label_partitioner.partitionCyclically();
    //k_label_partitioner.printGroups();

    //GraphParser graphParser;
    //graphParser.convertDotToMetis("./miao.dot");

    return 0;
}