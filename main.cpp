#include <iostream>
#include <KLabelPartitioner.h>
#include <boost/graph/graph_utility.hpp>
#include <stack>
#include "include/InputManager.h"
#include "include/GraphParser.h"
#include "src/SpectralClustering/SpectralClustering.h"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

/*int main() {
    //InputManager inputManager;

    //inputManager.readMeshFromOff("../data/bunny.off");
    //inputManager.readMeshFromOff("./cube_quad.off");
    //Graph g = inputManager.meshToGraphDual();

    Graph g(11);
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
    //boost::print_graph(g);

    KLabelPartitioner k_label_partitioner(g);

    std::vector<int> MIAO {1,1,2,3,3,2,3,1,3,1,1};
    k_label_partitioner.assignLabels(MIAO);
    //k_label_partitioner.clusterRecursively();
    k_label_partitioner.clusterCyclically();
    //k_label_partitioner.printClusters();

    //GraphParser graphParser;
    //graphParser.convertDotToMetis("./miao.dot");

    return 0;
}*/
#include <boost/filesystem.hpp>

int main() {
    /*int size = 3;
    Eigen::MatrixXf m = Eigen::MatrixXf::Zero(size,size);
    for (unsigned int i=0; i < size; i++) {
        for (unsigned int j=0; j < size; j++) {
            // generate similarity
                    int arg = pow(i-j,2);
            float similarity = exp(-arg);
            m(i,j) = similarity;
            m(j,i) = similarity;
        }
    }
    SpectralClustering mySpecter(m,1);
    mySpecter.setupEigenvectors();
    mySpecter.clusterKmeans(2);*/
}