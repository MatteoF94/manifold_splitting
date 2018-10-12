//
// Created by matteo on 02/10/18.
//
//std::string input_filename = "/home/matteo/Desktop/filtered/filtered/Labeling_Kitti0095f__Local__8.1_0.2_0.2_1_filtered.off";
#include <cstdio>
#include <InputManager.h>
#include <stopwatch.h>
#include <fstream>
#include <MeshManager.h>
#include <KLabelPartitioner.h>
#include <KSizeClustering.h>
#include <SpectralClustering.h>
#include <opencv2/core.hpp>
#include <eigen3/Eigen/Core>

std::string selectMesh(int selector) {
    std::string selected;
    switch (selector) {
        case 0:
            selected = "/home/matteo/Desktop/filtered/filtered/Labeling_Kitti0095f__Local__8.1_0.2_0.2_1_filtered.off";
            break;
        case 1:
            selected = "/home/matteo/Desktop/filtered/filtered/Labeling_FountainProposed_Local__0.95_0.2_0.2_1_filtered.off";
            break;
        case 2:
            selected = "/home/matteo/Desktop/filtered/filtered/Labeling_SouthbuildingOverl_Local__0.2_0.2_0.2_1_filtered.off";
            break;
        case 3:
            selected = "/home/matteo/Desktop/filtered/filtered/Labeling_Daghsthul_Local__0.4_0.2_0.2_1_filtered.off";
            break;
        default:
            break;
    }

    return selected;
}

int main (int argc, char* argv[]) {

   /*VertexInfo a,b,c,d;
    a.label = 0;
    a.nodes = {1};
    a.adjacent_groups = {1,3};
    b.label = 1;
    b.nodes = {2,3};
    b.adjacent_groups = {0,2};
    c.label = 2;
    c.nodes = {4,5,6};
    c.adjacent_groups = {1,3};
    d.label = 3;
    d.nodes = {7,8};
    d.adjacent_groups = {0,2};
    std::vector<VertexInfo> info = {a,b,c,d};

    KSizeClustering clusterer;
    std::vector<VertexInfo> hello = clusterer.reduceGraph(info);
    int miao = 0;
    exit(0);*/

    std::string input_filename = selectMesh(0);

    InputManager input_manager;
    MeshManager mesh_manager;
    KSizeClustering k_size_clustering;
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

    std::cout << "Extracting labels from mesh..." << std::endl;
    stopwatch.start();
    std::vector<int> labels = mesh_manager.getMeshLabels(mesh);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    KLabelPartitioner label_partitioner(g);
    label_partitioner.assignLabels(labels);
    std::cout << "Label partitioning..." << std::endl;
    stopwatch.start();
    label_partitioner.clusterCyclically();
    std::vector<VertexInfo> clusters = label_partitioner.createClusters();
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;
    std::cout << "Number of clusters from label partitioning: " << clusters.size() << std::endl;

    std::cout << "High order clustering..." << std::endl;
    stopwatch.start();
    std::vector<VertexInfo> high_clusters = k_size_clustering.reduceGraph(clusters);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    // Medium clustering
    std::vector<int> high_clusters_labels(high_clusters.size());
    int num_edges = 0;

    for(auto &curr_cluster : high_clusters) {
        for(auto &curr_node : curr_cluster.adjacent_groups) {
            high_clusters_labels.at(curr_node) = curr_cluster.label;
            ++num_edges;
        }
    }

    std::string vertex_graph_filename("../../data/vertex_info.graph");
    std::ofstream vertex_outfile(vertex_graph_filename);

    vertex_outfile << high_clusters.size() << " " << num_edges/2 << std::endl;
    for(auto &curr_cluster : high_clusters) {
        for(auto &curr_node : curr_cluster.adjacent_groups) {
            vertex_outfile << curr_node+1 << " ";
        }
        vertex_outfile << std::endl;
    }

    std::cout << "Partitioning the finite dual graph..." << std::endl;
    std::string execution_call = std::string("/home/matteo/GithubRepos/KaHIP-master/deploy/kaffpa") +
                                 std::string(" ../../data/vertex_info.graph") +
                                 std::string(" --k 8") +
                                 std::string(" --preconfiguration=strong") +
                                 std::string(" --output_filename ../../data/vertex_info.txt");
    stopwatch.start();
    system(execution_call.c_str());
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::string vertex_partition_filename("../../data/vertex_info.txt");
    std::ifstream vertex_partition_infile(vertex_partition_filename);
    std::vector<int> groups;
    std::string line;
    while(std::getline(vertex_partition_infile,line)) {
        std::istringstream iss(line);
        int number;
        iss >> number;
        groups.push_back(number);
    }

    std::vector<int> tmp(mesh.num_faces());
    for (int i = 0; i < high_clusters.size(); ++i) {
        VertexInfo curr_group = high_clusters.at(i);
        for (auto &node : curr_group.nodes) {
            tmp.at(node) = groups.at(i);
        }
    }

    std::ofstream final_outfile("../../data/kitti095.txt");
    for (int i = 0; i < tmp.size(); ++i)
        final_outfile << tmp.at(i) << std::endl;


    /*Eigen::MatrixXf test_labels = Eigen::MatrixXf::Identity(high_clusters.size(),high_clusters.size());

    std::cout << "Creating similarity matrix..." << std::endl;
    stopwatch.start();
    for (int i = 0; i < high_clusters.size(); i++) {
        VertexInfo curr_group = high_clusters.at(i);
        for (int elem: curr_group.adjacent_groups) {
            float tmp;
            if(curr_group.nodes.size() > 50000 && high_clusters.at(elem).nodes.size() > 50000)
                tmp = 0.05f;
            else
                tmp = 0.5f;
            test_labels(i,elem) = tmp;
            test_labels(elem,i) = tmp;

        }
    }
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    // Compute spectral clustering as second phase
    int num_clusters = 4;
    std::cout << "Spectral clustering, K = " << num_clusters << "..." << std::endl;
    stopwatch.start();
    SpectralClustering spectral_clustering(test_labels,num_clusters);
    spectral_clustering.setupEigenvectors();
    cv::Mat phase2_labels = spectral_clustering.clusterKmeans(num_clusters);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    VertexInfo phase2_clusters[num_clusters];

    // Create the new clusters
    std::cout << "Cluster creation..." << std::endl;
    stopwatch.start();
    for (int i = 0; i < high_clusters.size(); i++) {
        VertexInfo curr_vertex = high_clusters.at(i);
        int curr_group =  phase2_labels.at<int>(i);
        phase2_clusters[curr_group].nodes.insert(phase2_clusters[curr_group].nodes.end(),curr_vertex.nodes.begin(),curr_vertex.nodes.end());
    }
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int> face_map;
    for (int i = 0; i < num_clusters; ++i) {
        VertexInfo elem = phase2_clusters[i];
        for (auto &fd : elem.nodes) {
            boost::graph_traits<Mesh>::face_descriptor curr_desc(fd);
            face_map.insert({curr_desc,i});
        }
    }

    std::string partition_filename = "../../data/daghsthul.txt";
    std::ofstream outfile(partition_filename);
    for(auto fd : faces(mesh)) {
        outfile << face_map.at(fd) << std::endl;
    }*/

    /*std::vector<int> sizes;
    for (int i = 0; i < high_clusters.size(); ++i) {
        sizes.push_back(high_clusters.at(i).nodes.size());
    }

    std::vector<int> faces(mesh.num_faces());
    for (int i = 0; i < high_clusters.size(); ++i) {
        VertexInfo curr_vertex = high_clusters.at(i);
        for(auto &face : curr_vertex.nodes) {
            faces.at(face) = curr_vertex.label;
        }
    }

    //std::ofstream final("../../data/fountain.txt");
    std::ofstream final("../../data/daghsthul.txt");
    for (int i = 0; i < faces.size(); ++i)
        final << faces.at(i) << std::endl;*/

    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    //std::vector<Mesh> meshes_n = mesh_manager.breakMesh(mesh,std::string("../../data/fountain.txt"));
    std::vector<Mesh> meshes_n = mesh_manager.breakMesh(mesh,std::string("../../data/kitti095.txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    input_manager.writeMeshToOff(meshes_n,std::string("../../data/KITTI095/kitti095"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;

    exit(0);


    // Medium clustering
    /*std::vector<int> high_clusters_labels(high_clusters.size());
    int num_edges = 0;

    for(auto &curr_cluster : high_clusters) {
        for(auto &curr_node : curr_cluster.adjacent_groups) {
            high_clusters_labels.at(curr_node) = curr_cluster.label;
            ++num_edges;
        }
    }

    std::string vertex_graph_filename("../../data/vertex_info.graph");
    std::ofstream vertex_outfile(vertex_graph_filename);

    vertex_outfile << high_clusters.size() << " " << num_edges/2 << std::endl;
    for(auto &curr_cluster : high_clusters) {
        for(auto &curr_node : curr_cluster.adjacent_groups) {
            vertex_outfile << curr_node+1 << " ";
        }
        vertex_outfile << std::endl;
    }

    std::cout << "Partitioning the finite dual graph..." << std::endl;
    std::string execution_call = std::string("/home/matteo/GithubRepos/KaHIP-master/deploy/kaffpa") +
                                 std::string(" ../../data/vertex_info.graph") +
                                 std::string(" --k 10") +
                                 std::string(" --preconfiguration=strong") +
                                 std::string(" --output_filename ../../data/vertex_info.txt");
    stopwatch.start();
    system(execution_call.c_str());
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::string vertex_partition_filename("../../data/vertex_info.txt");
    std::ifstream vertex_partition_infile(vertex_partition_filename);
    std::vector<int> groups;
    std::string line;
    while(std::getline(vertex_partition_infile,line)) {
        std::istringstream iss(line);
        int number;
        iss >> number;
        groups.push_back(number);
    }

    std::vector<int> tmp(mesh.num_faces());
    for (int i = 0; i < high_clusters.size(); ++i) {
        VertexInfo curr_group = high_clusters.at(i);
        for (auto &node : curr_group.nodes) {
            tmp.at(node) = groups.at(i);
        }
    }

    std::ofstream final_outfile("../../data/try.txt");
    for (int i = 0; i < tmp.size(); ++i)
        final_outfile << tmp.at(i) << std::endl;*/

    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/try.txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    input_manager.writeMeshToOff(meshes,std::string("../../data/KITTI095/"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;

    /*std::string vertex_input("../../data/vertex_info.txt");
    std::ifstream vertex_infile(vertex_input);
    std::vector<int> groups;
    std::string line;
    while(std::getline(vertex_infile,line)) {
        std::istringstream iss(line);
        int number;
        iss >> number;
        groups.push_back(number);
    }

    // Merge groups
    std::vector<std::vector<VertexInfo>> new_groups(100);
    for (int i = 0; i < high_clusters.size(); ++i) {
        VertexInfo curr_node = high_clusters.at(i);
        new_groups.at(groups.at(i)).push_back(curr_node);
    }

    std::vector<VertexInfo> final_groups;
    for (int i = 0; i < 100; ++i) {
        VertexInfo new_node;

    }*/

    exit(0);














    // Similarity for the matrix
    /*Eigen::MatrixXf test_labels = Eigen::MatrixXf::Identity(high_clusters.size(),high_clusters.size());

    std::cout << "Creating similarity matrix..." << std::endl;
    stopwatch.start();
    for (int i = 0; i < high_clusters.size(); i++) {
        VertexInfo curr_group = high_clusters.at(i);
        for (int elem: curr_group.adjacent_groups) {
            float tmp = 1 - (std::abs(curr_group.nodes.size()-high_clusters.at(elem).nodes.size()) / (curr_group.nodes.size()+high_clusters.at(elem).nodes.size()));
            if(tmp < 0.1) tmp = 0.1;
            test_labels(i,elem) = tmp;
            test_labels(elem,i) = tmp;

        }
    }
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

// Compute spectral clustering as second phase
    int num_clusters = 12;
    std::cout << "Spectral clustering, K = " << num_clusters << "..." << std::endl;
    stopwatch.start();
    SpectralClustering spectral_clustering(test_labels,num_clusters);
    spectral_clustering.setupEigenvectors();
    cv::Mat phase2_labels = spectral_clustering.clusterKmeans(num_clusters);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    VertexInfo phase2_clusters[num_clusters];

    // Create the new clusters
    std::cout << "Cluster creation..." << std::endl;
    stopwatch.start();
    for (int i = 0; i < high_clusters.size(); i++) {
        VertexInfo curr_vertex = high_clusters.at(i);
        int curr_group =  phase2_labels.at<int>(i);
        phase2_clusters[curr_group].nodes.insert(phase2_clusters[curr_group].nodes.end(),curr_vertex.nodes.begin(),curr_vertex.nodes.end());
    }
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;


    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int> face_map;
    for (int i = 0; i < num_clusters; ++i) {
        VertexInfo elem = phase2_clusters[i];
        for (auto &fd : elem.nodes) {
            boost::graph_traits<Mesh>::face_descriptor curr_desc(fd);
            face_map.insert({curr_desc,i});
        }
    }

    std::string partition_filename = "../../data/try.txt";
    std::ofstream outfile(partition_filename);
    for(auto fd : faces(mesh)) {
        outfile << face_map.at(fd) << std::endl;
    }

    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/try.txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    input_manager.writeMeshToOff(meshes,std::string("../../data/KITTI095/"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;*/
}