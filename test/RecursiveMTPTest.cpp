//
// Created by matteo on 13/09/18.
//
#include <MultiTreePartitioner.h>
#include <cstdio>
#include <InputManager.h>
#include <stopwatch.h>
#include <fstream>
#include <MultiTreeManager.h>
#include <MeshManager.h>

void changeFaceMapValues(std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int>* map, int new_value) {

    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int>::iterator it = map->begin();

    while(it != map->end()) {
        if(it->second == 0)
            it->second = new_value;
        else
            it->second = new_value + 1;

        ++it;
    }
}

void fillMapByID(MultiTreeNode* root, std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int>* map, std::vector<int> ids) {

    while(root != nullptr) {
        map->operator[](root->id) = ids.at(root->id);
        root = root->next;
    }
}

void printGivenLevel(MultiTreeNode* root, int level) {
    if (root == nullptr)
        return;
    if(root->level == level)
        std::cout << root->id << " ";
    else {
        printGivenLevel(root->left,level);
        printGivenLevel(root->right,level);
        printGivenLevel(root->mid,level);
    }
}

void printLevelOrder(MultiTreeNode* root) {
    int h = 0;
    for (int i = 100; i>=h; --i) {
        printGivenLevel(root,i);
        std::cout << std::endl;
    }
}

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

    std::string selected_mesh = selectMesh(8);
    std::string input_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".off";

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

    int K = 2;
    int thresh = num_nodes / K;
    MultiTreePartitioner partitioner;
    partitioner.configParameters(10,thresh,20,K);

    std::cout << "Partitioning main tree..." << std::endl;
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
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    //exit(0);
    /*------------------------------------------------------------------------------*/
    std::cout << "-- FIRST GENERATION TREES --" << std::endl;

    std::vector<std::vector<MultiTreeNode*>*>* roots_set = partitioner.getRoots();
    std::vector<MultiTreeNode *> *curr_roots_set_first = roots_set->at(0);
    std::vector<MultiTreeNode *> miao = *curr_roots_set_first;
    std::vector<MultiTreeNode *> *curr_roots_set_second = roots_set->at(1);

    std::cout << "Regenerating first subtree..." << std::endl;
    stopwatch.start();
    tree_manager.linkTrees(curr_roots_set_first);
    tree_manager.regenerateTree(curr_roots_set_first->front(),group_ids);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Regenerating second subtree..." << std::endl;
    stopwatch.start();
    tree_manager.linkTrees(curr_roots_set_second);
    tree_manager.regenerateTree(curr_roots_set_second->front(),group_ids);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    MultiTreeNode *curr_last_first = curr_roots_set_first->front();
    MultiTreeNode *curr_last_second = curr_roots_set_second->front();
    int g1_num = 1, g2_num = 1, g3_num = 1, g4_num = 1;
    while(curr_last_first->next != nullptr) {
        curr_last_first = curr_last_first->next;
        ++g1_num;
    }
    while(curr_last_second->next != nullptr) {
        curr_last_second = curr_last_second->next;
        ++g2_num;
    }

    partitioner.configParameters(10,g1_num/2,20,K);
    g1_num = 1;
    std::cout << "Partitioning first subtree..." << std::endl;
    stopwatch.start();
    std::vector<int> group_ids_first = partitioner.partitionByNumber(curr_last_first, curr_roots_set_first->front(), num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    num_elem.at(0) = 0;
    num_elem.at(1) = 0;
    for (int group_id : group_ids_first) {
        if (group_id != -1)
            num_elem.at(group_id)++;
    }
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    std::vector<std::vector<MultiTreeNode*>*>* roots_set_first = partitioner.getRoots();

    partitioner.configParameters(10,g2_num/2,20,K);
    g2_num = 1;
    std::cout << "Partitioning second subtree..." << std::endl;
    stopwatch.start();
    std::vector<int> group_ids_second = partitioner.partitionByNumber(curr_last_second, curr_roots_set_second->front(), num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    num_elem.at(0) = 0;
    num_elem.at(1) = 0;
    for (int group_id : group_ids_second) {
        if (group_id != -1)
            num_elem.at(group_id)++;
    }
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    std::vector<std::vector<MultiTreeNode*>*>* roots_set_second = partitioner.getRoots();
   // exit(0);
    /*--------------------------------------------------------------------------------------------*/
    std::cout << "-- SECOND GENERATION TREES --" << std::endl;

    curr_roots_set_first = roots_set_first->at(0);
    curr_roots_set_second = roots_set_first->at(1);
    int curr_group = group_ids_first.at(curr_roots_set_first->front()->id);
    curr_group = group_ids_first.at(curr_roots_set_second->front()->id);

    std::cout << "Regenerating first subsubtree..." << std::endl;
    stopwatch.start();
    tree_manager.linkTrees(curr_roots_set_first);
    tree_manager.regenerateTree(curr_roots_set_first->front(),group_ids_first);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Regenerating second subsubtree..." << std::endl;
    stopwatch.start();
    tree_manager.linkTrees(curr_roots_set_second);
    tree_manager.regenerateTree(curr_roots_set_second->front(),group_ids_first);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    curr_last_first = curr_roots_set_first->front();
    curr_last_second = curr_roots_set_second->front();
    while(curr_last_first->next != nullptr) {
        curr_last_first = curr_last_first->next;
        ++g1_num;
    }
    while(curr_last_second->next != nullptr) {
        curr_last_second = curr_last_second->next;
        ++g2_num;
    }

    partitioner.configParameters(10,g1_num/2,20,K);
    std::cout << "Partitioning first subsubtree..." << std::endl;
    stopwatch.start();
    group_ids_first = partitioner.partitionByNumber(curr_last_first, curr_roots_set_first->front(), num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    num_elem.at(0) = 0;
    num_elem.at(1) = 0;
    for (int group_id : group_ids_first) {
        if (group_id != -1)
            num_elem.at(group_id)++;
    }
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int> first_map;
    fillMapByID(curr_roots_set_first->front(),&first_map,group_ids_first);

    partitioner.configParameters(10,g2_num/2,20,K);
    std::cout << "Partitioning second subsubtree..." << std::endl;
    stopwatch.start();
    group_ids_first = partitioner.partitionByNumber(curr_last_second, curr_roots_set_second->front(), num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    num_elem.at(0) = 0;
    num_elem.at(1) = 0;
    for (int group_id : group_ids_first) {
        if (group_id != -1)
            num_elem.at(group_id)++;
    }
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int> second_map;
    fillMapByID(curr_roots_set_second->front(),&second_map,group_ids_first);

    /*----------------------------------------------------------------------------------------------*/

    curr_roots_set_first = roots_set_second->at(0);
    curr_roots_set_second = roots_set_second->at(1);

    std::cout << "Regenerating third subsubtree..." << std::endl;
    stopwatch.start();
    tree_manager.linkTrees(curr_roots_set_first);
    tree_manager.regenerateTree(curr_roots_set_first->front(),group_ids_second);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Regenerating fourth subsubtre..." << std::endl;
    stopwatch.start();
    tree_manager.linkTrees(curr_roots_set_second);
    tree_manager.regenerateTree(curr_roots_set_second->front(),group_ids_second);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    curr_last_first = curr_roots_set_first->front();
    curr_last_second = curr_roots_set_second->front();
    while(curr_last_first->next != nullptr) {
        curr_last_first = curr_last_first->next;
        ++g3_num;
    }
    while(curr_last_second->next != nullptr) {
        curr_last_second = curr_last_second->next;
        ++g4_num;
    }

    partitioner.configParameters(10,g3_num/2,20,K);
    std::cout << "Partitioning third subsubtree..." << std::endl;
    stopwatch.start();
    group_ids_second = partitioner.partitionByNumber(curr_last_first, curr_roots_set_first->front(), num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    num_elem.at(0) = 0;
    num_elem.at(1) = 0;
    for (int group_id : group_ids_second) {
        if (group_id != -1)
            num_elem.at(group_id)++;
    }
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int> third_map;
    fillMapByID(curr_roots_set_first->front(),&third_map,group_ids_second);

    partitioner.configParameters(10,g4_num/2,10,K);
    std::cout << "Partitioning fourth subsubtree..." << std::endl;
    stopwatch.start();
    group_ids_second = partitioner.partitionByNumber(curr_last_second, curr_roots_set_second->front(), num_nodes);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    num_elem.at(0) = 0;
    num_elem.at(1) = 0;
    for (int group_id : group_ids_second) {
        if (group_id != -1)
            num_elem.at(group_id)++;
    }
    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;
    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int> fourth_map;
    fillMapByID(curr_roots_set_second->front(),&fourth_map,group_ids_second);
//exit(0);
    changeFaceMapValues(&second_map,2);
    first_map.insert(second_map.begin(),second_map.end());
    changeFaceMapValues(&third_map,4);
    first_map.insert(third_map.begin(),third_map.end());
    changeFaceMapValues(&fourth_map,6);
    first_map.insert(fourth_map.begin(),fourth_map.end());

    std::string partition_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + "_rmtp.txt";
    std::ofstream outfile(partition_filename);
    int min_pos = static_cast<int>(std::distance(num_elem.begin(), std::min_element(num_elem.begin(), num_elem.end())));
    for (boost::graph_traits<Mesh>::face_descriptor fd : mesh.faces()) {
            outfile << first_map[fd] << std::endl;
    }

    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + "_rmtp.txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    input_manager.writeMeshToOff(meshes,std::string("../../data/Watermarking/" + selected_mesh + "/RMTP/" + selected_mesh));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;
}