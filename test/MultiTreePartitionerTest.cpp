//
// Created by matteo on 10/08/18.
//
#include <MultiTreePartitioner.h>
#include <cstdio>
#include <InputManager.h>
#include <stopwatch.h>
#include <fstream>
#include <MultiTreeManager.h>
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
        default:
            break;
    }
}

int main (int argc, char* argv[]) {

    std::string selected_mesh = selectMesh(0);
    //std::string input_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".off";
    std::string input_filename("/home/matteo/Desktop/meshes/Castle.off");


    InputManager input_manager;
    MeshManager mesh_manager;
    MultiTreeManager tree_manager;
    Stopwatch stopwatch;
    double elapsed_time, total_time = 0;

    std::cout << "Reading mesh from .off file..." << std::endl;
    stopwatch.start();
    Mesh mesh = input_manager.readMeshFromOff(input_filename);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Converting mesh to multi level tree..." << std::endl;
    stopwatch.start();
    MultiTreeNode* root = tree_manager.meshToTree(mesh, MultiTreeManager::CreationMode::BALANCED,100);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    MultiTreeNode* last = root;
    int num_nodes = input_manager.getNumFaces();
    while (last->next != nullptr) {
        last = last->next;
    }

    int K = 8;
    int thresh = num_nodes / K;
    MultiTreePartitioner partitioner;
    partitioner.configParameters(10,thresh,15,K);

    std::cout << "Partitioning tree..." << std::endl;
    stopwatch.start();
    std::vector<int> group_ids = partitioner.partitionByNumber(last, root, num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partitioning results: " << std::endl;
    std::vector<int> num_elem(K,0);

    for (int group_id : group_ids) {
        if(group_id != -1)
            num_elem.at(group_id)++;
    }


//exit(0);
    //std::string partition_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + "_mtp.txt";
    std::string partition_filename = "../../data/castle_mtp.txt";

    std::ofstream outfile(partition_filename);
    int min_pos = static_cast<int>(std::distance(num_elem.begin(), std::min_element(num_elem.begin(), num_elem.end())));
    for (int group_id : group_ids) {
        if(group_id == -1)
            outfile << min_pos << std::endl;
        else
            outfile << group_id << std::endl;
    }

    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
//exit(0);
    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    //std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + "_mtp.txt"));
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/castle_mtp.txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    //input_manager.writeMeshToOff(meshes,std::string("../../data/Watermarking/" + selected_mesh + "/MTP/" + selected_mesh));
    input_manager.writeMeshToOff(meshes,std::string("../../data/Castle/castle_mpt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;
}
