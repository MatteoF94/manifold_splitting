//
// Created by matteo on 28/07/18.
//

#include <string>
#include <fstream>
#include <InputManager.h>
#include <stopwatch.h>
#include <boost/graph/graphviz.hpp>
#include <GraphParser.h>

int main (int argc, char* argv[]) {

    //std::string input_filename = "../../data/WatermarkingBenchmark/bunny.off";
    //std::string input_filename = "../../data/EPFL/castle_dense_large/castle_dense_large.off";
    std::string input_filename = "../../data/EPFL/fountain-P11/fountain.off";
    InputManager input_manager;
    GraphParser graph_parser;
    Stopwatch stopwatch;
    double elapsed_time, total_time = 0;

    input_manager.readMeshFromOff(input_filename);

    std::cout << "Converting mesh to finite dual..." << std::endl;
    stopwatch.start();
    Graph g = input_manager.meshToGraphDual();
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    //std::string output_filename = "../../data/WatermarkingBenchmark/bunny.dot";
    //std::string output_filename = "../../data/EPFL/castle_dense_large/castle_dense_large.dot";
    std::string output_filename = "../../data/EPFL/fountain-P11/fountain.dot";
    std::ofstream dot_file(output_filename);
    boost::write_graphviz(dot_file,g);
    std::cout << "Converting graph from .DOT to .graph (METIS format)..." << std::endl;
    stopwatch.start();
    graph_parser.convertDotToMetis(output_filename);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partitioning the finite dual graph..." << std::endl;
    std::string execution_call = std::string("/home/matteo/GithubRepos/KaHIP-master/deploy/kaffpa") +
                                 //std::string(" ../../data/WatermarkingBenchmark/bunny.graph") +
                                 //std::string(" ../../data/EPFL/castle_dense_large/castle_dense_large.graph") +
                                 std::string(" ../../data/EPFL/fountain-P11/fountain.graph") +
                                 std::string(" --k 8") +
                                 std::string(" --preconfiguration=strong") +
                                 //std::string(" --output_filename ../../data/WatermarkingBenchmark/bunny.txt");
                                 //std::string(" --output_filename ../../data/EPFL/castle_dense_large/castle_dense_large.txt");
                                 std::string(" --output_filename ../../data/EPFL/fountain-P11/fountain.txt");
    std::cout << execution_call << std::endl;
    stopwatch.start();
    system(execution_call.c_str());
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Saving the mesh partitions..." << std::endl;
    stopwatch.start();
    //input_manager.breakMesh(8,std::string("../../data/WatermarkingBenchmark/bunny.txt"),std::string("../../data/WatermarkingBenchmark/bunnyOut/bunny"));
    //input_manager.breakMesh(8,std::string("../../data/EPFL/castle_dense_large/castle_dense_large.txt"),std::string("../../data/EPFL/castle_dense_large/out/castle"));
    input_manager.breakMesh(8,std::string("../../data/EPFL/fountain-P11/fountain.txt"),std::string("../../data/EPFL/fountain-P11/out/fountain"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;
}

