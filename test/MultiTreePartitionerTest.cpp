//
// Created by matteo on 10/08/18.
//
#include <MultiTreePartitioner.h>
#include <cstdio>
#include <InputManager.h>
#include <stopwatch.h>
#include <fstream>

int main (int argc, char* argv[]) {

    Stopwatch stopwatch;
    double elapsed_time = 0.0;

    InputManager input_manager;
    //std::string input_filename = "../../data/WatermarkingBenchmark/Ramesses.off";
    //std::string input_filename = "../../data/EPFL/castle_dense_large/castle_dense_large.off";
    std::string input_filename = "../../data/EPFL/fountain-P11/fountain.off";
    input_manager.readMeshFromOff("../../data/Manifold_Mesh.off");//input_filename);

    std::cout << "\nConverting mesh to multi level tree..." << std::endl;
    stopwatch.start();
    MultiTreeNode* root = input_manager.meshToMultiTree(0,50);
    elapsed_time = stopwatch.stop();
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    MultiTreeNode* last = root;
    int num_nodes = input_manager.getNumFaces();
    while (last->next != nullptr) {
        last = last->next;
    }

    int K = 8;
    int thresh = num_nodes / K;
    MultiTreePartitioner partitioner;
    partitioner.configParameters(20,thresh,20,K);

    std::cout << "Partitioning tree..." << std::endl;
    stopwatch.start();
    std::vector<int> group_ids = partitioner.partitionByNumber(last, root, num_nodes);
    elapsed_time = stopwatch.stop();
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partitioning results: " << std::endl;
    std::vector<int> num_elem(K,0);

    for (int group_id : group_ids) {
        if(group_id != -1)
            num_elem.at(group_id)++;
    }

    int min_pos = std::distance(num_elem.begin(),std::min_element(num_elem.begin(),num_elem.end()));

    //std::string partition_filename = "../../data/EPFL/castle_dense_large/castle_dense_large_M.txt";
    std::string partition_filename = "../../data/EPFL/fountain-P11/fountain_M.txt";
    std::ofstream txt_file(partition_filename);
    for (int group_id : group_ids) {
        if(group_id == -1)
            txt_file << min_pos << std::endl;
        else
            txt_file << group_id << std::endl;
    }

    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;

    std::cout << "Saving the mesh partitions..." << std::endl;
    stopwatch.start();
    //input_manager.breakMesh(8,std::string("../../data/EPFL/castle_dense_large/castle_dense_large_M.txt"),std::string("../../data/EPFL/castle_dense_large/out_M/castle"));
    input_manager.breakMesh(8,std::string("../../data/EPFL/fountain-P11/fountain_M.txt"),std::string("../../data/EPFL/fountain-P11/out_M/fountain"));
    elapsed_time = stopwatch.stop();
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;
}
