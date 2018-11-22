//
// Created by matteo on 28/07/18.
//

#include <string>
#include <fstream>
#include <InputManager.h>
#include <stopwatch.h>
#include <boost/graph/graphviz.hpp>
#include <GraphParser.h>
#include <MeshManager.h>

std::string selectMesh(int mesh_idx) {

    switch (mesh_idx) {
        case 0:
            return "bunny";
        case 1:
            return "casting";
        case 2:
            return "cow";
        case 3:
            return "crank";
        case 4:
            return "dragon";
        case 5:
            return "hand";
        case 6:
            return "horse";
        case 7:
            return "rabbit";
        case 8:
            return "ramesses";
        case 9:
            return "venus";
        default:break;
    }
}

int main (int argc, char* argv[]) {

    std::string selected_mesh = selectMesh(0);
    std::string input_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".off";
    //std::string input_filename = "../../data/EPFL/castle_dense_large/castle_dense_large.off";
    //std::string input_filename = "../../data/EPFL/fountain-P11/fountain.off";
    InputManager input_manager;
    MeshManager mesh_manager;
    GraphParser graph_parser;
    Stopwatch stopwatch;
    double elapsed_time, total_time = 0;

    std::cout << "Reading mesh from .off file..." << std::endl;
    stopwatch.start();
    Mesh mesh = input_manager.readMeshFromOff(input_filename);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Converting mesh to finite dual..." << std::endl;
    stopwatch.start();
    Graph g = mesh_manager.meshToGraph(mesh);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::string output_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".dot";
    //std::string output_filename = "../../data/EPFL/castle_dense_large/castle_dense_large.dot";
    //std::string output_filename = "../../data/EPFL/fountain-P11/fountain.dot";
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
                                 std::string(" ../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".graph") +
                                 //std::string(" ../../data/EPFL/castle_dense_large/castle_dense_large.graph") +
                                 //std::string(" ../../data/EPFL/fountain-P11/fountain.graph") +
                                 std::string(" --k 8") +
                                 std::string(" --preconfiguration=strong") +
                                 std::string(" --output_filename ../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".txt");
                                 //std::string(" --output_filename ../../data/EPFL/castle_dense_large/castle_dense_large.txt");
                                 //std::string(" --output_filename ../../data/EPFL/fountain-P11/fountain.txt");
    stopwatch.start();
    system(execution_call.c_str());
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    input_manager.writeMeshToOff(meshes,std::string("../../data/Watermarking/" + selected_mesh + "/KaHIP/" + selected_mesh));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;
}

