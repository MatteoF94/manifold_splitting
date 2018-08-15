//
// Created by matteo on 10/08/18.
//
#include <MultiTreePartitioner.h>
#include <cstdio>
#include <InputManager.h>
#include <stopwatch.h>

int main (int argc, char* argv[]) {

    Stopwatch stopwatch;
    double elapsed_time = 0.0;

    InputManager input_manager;
    std::string input_filename = "../../data/WatermarkingBenchmark/bunny.off";
    input_manager.readMeshFromOff(input_filename);

    std::cout << "\nConverting mesh to multi level tree..." << std::endl;
    stopwatch.start();
    MultiTreeNode* root = input_manager.meshToMultiTree(0,100);
    elapsed_time = stopwatch.stop();
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    MultiTreeNode* last = root;
    int num_nodes = 1;
    while (last->next != nullptr) {
        last = last->next;
        num_nodes++;
    }

    int K = 8;
    int thresh = num_nodes / K;
    MultiTreePartitioner partitioner;
    partitioner.configParameters(0,thresh,30,K);

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

    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
}
