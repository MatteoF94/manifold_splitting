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
#include <MeshManager.h>
#include <MultiTreeManager.h>
#include <stopwatch.h>
#include <CGAL/Polygon_mesh_processing/stitch_borders.h>
#include <MultiTreePartitioner.h>

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

    InputManager miao;
    miao.readMeshFromOff("../data/WatermarkingBenchmark/bunny.off");
    /*MeshManager manager;
    Stopwatch stopwatch;
    stopwatch.start();
    manager.computeFacesArea(miao.getMesh());
    manager.meshToGraph(miao.getMesh());
    double elapsed = stopwatch.stop();
    std::cout << "Dual graph created in: " << elapsed << " seconds" << std::endl;
    stopwatch.start();
    manager.breakMesh(miao.getMesh(),"../data/WatermarkingBenchmark/bunny.txt");
    elapsed = stopwatch.stop();
    std::cout << "Mesh broken in: " << elapsed << " seconds" << std::endl;*/

    /*CGAL::Surface_mesh<Point> mesh;
    CGAL::Polygon_mesh_processing::stitch_borders(mesh);*/

    MultiTreeManager treeManager;
    MultiTreeNode* root1 = treeManager.meshToTree(miao.getMesh(),MultiTreeManager::CreationMode::BALANCED);
    MultiTreeNode* root2 = miao.meshToMultiTree(100);

    MultiTreeNode* root3 = treeManager.meshToTree(miao.getMesh(),MultiTreeManager::CreationMode::THIN);

   /* std::vector<int> r1,r2;
    while(root1 != nullptr) {
        r1.push_back(root1->id);
        root1 = root1->next;
    }
    while(root2 != nullptr) {
        r2.push_back(root2->id);
        root2 = root2->next;
    }

    while(root3 != nullptr) {
        if(root3->parent != nullptr) {
            if (root3->right != nullptr || root3->mid != nullptr) {
                std::cout << "MARONNA" << std::endl;
            }
        }

        root3 = root3->next;
    }

    for (int i = 0; i < r1.size(); ++i) {
        if(r1.at(i) != r2.at(i)) {
            std::cout << "Maronna u' carmine" << std::endl;
            return 0;
        }
    }
    std::cout << "Yey same tree" << std::endl;*/

    MultiTreePartitioner multiTreePartitioner;
    multiTreePartitioner.configParameters(10,8725,10,8);
    MultiTreeNode* last = root3;
    int num_nodes = miao.getNumFaces();
    int i = 1;
    while (last->next != nullptr) {
        last = last->next;
        i++;
    }

    std::vector<int> group_ids = multiTreePartitioner.partitionThinByNumber(last, root3, num_nodes);

    std::cout << "Partitioning results: " << std::endl;
    std::vector<int> num_elem(8,0);

    for (int group_id : group_ids) {
        if(group_id != -1)
            num_elem.at(group_id)++;
    }

    for (int j = 0; j < 8; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
}