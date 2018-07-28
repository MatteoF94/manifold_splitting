//
// Created by matteo on 25/07/18.
//

#include <iostream>
#include <opencv2/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <InputManager.h>
#include <KLabelPartitioner.h>
#include <fstream>
#include <eigen3/Eigen/Core>
#include <SpectralClustering.h>
#include <dirent.h>

Graph graphFromImage(cv::Mat image) {
    auto num_rows = static_cast<unsigned int>(image.rows);
    auto num_cols = static_cast<unsigned int>(image.cols);

    Graph g(num_rows*num_cols);

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if(j+1 < num_cols)
                boost::add_edge(num_cols*i+j,num_cols*i+j+1,g);
            if(i+1 < num_rows)
                boost::add_edge(num_cols*i+j,num_cols*(i+1)+j,g);
            if(j-1 >= 0)
                boost::add_edge(num_cols*i+j,num_cols*i+j-1,g);
            if(i-1 >= 0)
                boost::add_edge(num_cols*i+j,num_cols*(i-1)+j,g);
        }
    }

    return g;
}

std::vector<VertexInfo> clusterByLabel(Graph g) {

    KLabelPartitioner partitioner(g);

    // Read the labels and assign them to the graph
    std::ifstream input("../../data/KLabelPartitionerTest/000120_10.txt");
    std::vector<int> labels;
    int tmp;
    while(input >> tmp)
        labels.push_back(tmp);

    partitioner.assignLabels(labels);

    // Cluster in a cyclical way, suggested for large scale graphs (>50K vertices)
    partitioner.clusterCyclically();
    std::vector<VertexInfo> clusters = partitioner.createClusters();

    return clusters;
}

void showPhase1Clusters(cv::Mat img_in, std::vector<VertexInfo> clusters) {
    auto num_rows = static_cast<unsigned int>(img_in.rows);
    auto num_cols = static_cast<unsigned int>(img_in.cols);

    cv::Mat img_mask = cv::Mat::zeros(num_rows,num_cols,CV_8UC1); // Not binary

    for (auto curr_cluster : clusters) {
        unsigned long num_elems = curr_cluster.nodes.size();

        for (unsigned long i = 0; i < num_elems; i++) {
            int curr_pixel_idx = curr_cluster.nodes.at(i);
            int col = curr_pixel_idx % num_cols;
            int row = curr_pixel_idx / num_cols;
            img_mask.at<uchar>(row, col) = 255;
        }

        cv::Mat img_bin, img_out;
        cv::threshold(img_out,img_bin,1,255,cv::THRESH_BINARY);
        img_in.copyTo(img_out,img_bin);

        cv::imshow("Phase 1 clustering (incremental images)",img_out);
        cv::waitKey(2000);
    }
}

std::unordered_map<int,int> createLabelIndexMap() {

    // Using the KITTI standard numerical notation for classification
    std::unordered_map<int,int> label_index_map;

    label_index_map.insert({4,0}); // lamppost lights
    label_index_map.insert({7,1}); // main road
    label_index_map.insert({9,2}); // parking spots
    label_index_map.insert({11,3}); //buildings
    label_index_map.insert({17,4}); // lamppost stand
    label_index_map.insert({19,5}); // high traffic light
    label_index_map.insert({20,6}); // road sign
    label_index_map.insert({21,7}); // trees
    label_index_map.insert({22,8}); // road pitch
    label_index_map.insert({23,9}); // sky
    label_index_map.insert({26,10}); // cars
    label_index_map.insert({27,11}); // trucks

    return label_index_map;
};

Eigen::MatrixXf createTestSimilarityMatrix() {

    Eigen::MatrixXf similarity_matrix = Eigen::MatrixXf::Identity(12,12);

    similarity_matrix(0,4) = 1;
    similarity_matrix(4,0) = 1;
    similarity_matrix(4,8) = 1;
    similarity_matrix(8,4) = 1;
    similarity_matrix(0,8) = 0.2;
    similarity_matrix(8,0) = 0.2;
    similarity_matrix(1,10) = 0.99;
    similarity_matrix(10,1) = 0.99;
    similarity_matrix(2,10) = 0.99;
    similarity_matrix(10,2) = 0.99;
    similarity_matrix(1,11) = 0.99;
    similarity_matrix(11,1) = 0.99;
    similarity_matrix(1,2) = 0.5;
    similarity_matrix(2,1) = 0.5;
    similarity_matrix(3,7) = 0.5;
    similarity_matrix(7,3) = 0.5;

    return similarity_matrix;
}

int main(int argc, char* argv[]) {

    bool show_phase1_clustering = false;
    if (argc == 2) {
        if(std::string(argv[1]) == "--SHOW_PHASE1")
            show_phase1_clustering = true;
        else {
            std::cerr << "Usage: " << argv[0] << "--SHOW_PHASE1" << std::endl;
            return 1;
        }
    }

    /*cv::Mat src = cv::imread("../../data/KLabelPartitionerTest/000120_10.png");
    Graph g = graphFromImage(src);

    std::vector<VertexInfo> phase1_clusters = clusterByLabel(g);

    if(show_phase1_clustering)
        showPhase1Clusters(src,phase1_clusters);


    // Prepare the custom similarity matrix for the specific test image
    std::unordered_map<int,int> label_index_map = createLabelIndexMap();
    Eigen::MatrixXf test_similarity_matrix = createTestSimilarityMatrix();

    // Create the similarity matrix for the clusters computed by the first phase of the algorithm
    Eigen::MatrixXf test_labels = Eigen::MatrixXf::Identity(phase1_clusters.size(),phase1_clusters.size());

    for (int i = 0; i < phase1_clusters.size(); i++) {
        VertexInfo curr_group = phase1_clusters.at(i);
        for (int elem: curr_group.adjacent_groups) {
            int row = label_index_map[curr_group.label];
            int col = label_index_map[phase1_clusters.at(elem).label];

            test_labels(i,elem) = test_similarity_matrix(row,col);
            test_labels(elem,i) = test_similarity_matrix(row,col);

        }
    }

    // Compute spectral clustering as second phase
    int num_clusters = 10;
    SpectralClustering spectral_clustering(test_labels,num_clusters);
    spectral_clustering.setupEigenvectors();
    cv::Mat phase2_labels = spectral_clustering.clusterKmeans(num_clusters);
    VertexInfo phase2_clusters[num_clusters];

    // Create the new clusters
    for (int i = 0; i < phase1_clusters.size(); i++) {
        VertexInfo curr_vertex = phase1_clusters.at(i);
        int curr_group =  phase2_labels.at<int>(i);
        phase2_clusters[curr_group].nodes.insert(phase2_clusters[curr_group].nodes.end(),curr_vertex.nodes.begin(),curr_vertex.nodes.end());
    }

    // Save clusters as images (they are groups of pixels after all!)
    // Clear the output directory
    std::string path = "../../data/KLabelPartitionerTest/clusters";
    DIR *p_dir;
    struct dirent *entry;
    if(p_dir==opendir(path.c_str())) {
        while(entry == readdir(p_dir)) {
            if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0) {
                std::remove(entry->d_name);
            }
        }
    }

    // Save the images
    for (int j = 0; j < num_clusters; j++) {
        cv::Mat firstClust = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
        VertexInfo myInfo = phase2_clusters[j];
        int num_elems = myInfo.nodes.size();
        for (int i = 0; i < num_elems; i++) {
            int curr_pixel_idx = myInfo.nodes.at(i);
            int col = curr_pixel_idx % src.cols;
            int row = curr_pixel_idx / src.cols;
            firstClust.at<uchar>(row, col) = 255;
        }
        cv::Mat out_img;
        cv::Mat bin_img;
        cv::threshold(firstClust,firstClust,200,255,cv::THRESH_BINARY);
        src.copyTo(out_img,firstClust);
        std::stringstream file_path;
        file_path << path << "/000120_10_" << std::to_string(j) << ".png";
        cv::imwrite(file_path.str().c_str(),out_img);
    }*/

    /*Eigen::MatrixXf test_labels = Eigen::MatrixXf::Identity(3,3);
    int num_clusters = 2;
    SpectralClustering spectral_clustering(test_labels,2);
    spectral_clustering.setupEigenvectors();
    cv::Mat phase2_labels = spectral_clustering.clusterKmeans(num_clusters);
    VertexInfo phase2_clusters[num_clusters];*/
    system("/home/matteo/GithubRepos/KaHIP-master/deploy/kaffpa /home/matteo/Desktop/gatto.graph --k 2 --preconfiguration=strong --output_filename /home/matteo/CLionProjects/manifold_splitting/include/miao.txt");
}
