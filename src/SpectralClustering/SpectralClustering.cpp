//
// Created by matteo on 24/07/18.
//

#include <iostream>
#include "SpectralClustering.h"
#include <eigen3/Eigen/Dense>
#include <opencv2/core/eigen.hpp>

SpectralClustering::SpectralClustering(Eigen::MatrixXf &similarity_matrix, int num_eigenvectors):
        mNumEigenVectors(num_eigenvectors),
        mNumClusters(0),
        mSimilarityMat(similarity_matrix) {
}

SpectralClustering::~SpectralClustering() {

}

void SpectralClustering::setupEigenvectors(){
    Eigen::MatrixXf deg = Eigen::MatrixXf::Zero(mSimilarityMat.rows(),mSimilarityMat.cols());
    Eigen::MatrixXf ones = Eigen::MatrixXf::Ones(mSimilarityMat.rows(),mSimilarityMat.cols());

    for (unsigned int i = 0; i < mSimilarityMat.cols(); i++) {
        deg(i,i) = 1.0/(sqrt((mSimilarityMat.row(i).sum())));
    }

    //Eigen::MatrixXf laplacian = deg*mSimilarityMat*deg;
            Eigen::MatrixXf laplacian = deg*mSimilarityMat;
    laplacian.noalias() = ones - laplacian*deg;
    std::cout << "My data: " << deg << std::endl;
    //std::cout << "My data: " << mSimilarityMat << std::endl;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> solver(laplacian);

    Eigen::VectorXf eigenvalues = solver.eigenvalues();
    Eigen::MatrixXf eigenvectors = solver.eigenvectors();

    long int num_eigenvalues = mSimilarityMat.cols();
    for (int i = 0; i < num_eigenvalues-1; i++) {
        int max_eigenvalue_idx;
        eigenvalues.segment(i,num_eigenvalues - i).maxCoeff(&max_eigenvalue_idx);

        if (max_eigenvalue_idx > 0) {
            //std::cout << eigenvalues[i] << std::endl;
            //std::cout << eigenvalues[i+max_eigenvalue_idx] << std::endl;
            std::swap(eigenvalues[i],eigenvalues[max_eigenvalue_idx+i]);
            eigenvectors.col(i).swap(eigenvectors.col(max_eigenvalue_idx+i));
        }
    }

    if(mNumEigenVectors < eigenvectors.cols()) {
        mEigenVectors = eigenvectors.block(0,0,eigenvectors.rows(),mNumEigenVectors);
    } else {
        mEigenVectors = eigenvectors;
    }

    mEigenVectors.rowwise().normalize();
}

cv::Mat SpectralClustering::clusterKmeans(int num_clusters) {
    // Prepare elements
    //std::cout << mEigenVectors.rows() << std::endl;
    //std::cout << mEigenVectors.cols() << std::endl;
    auto num_elements = static_cast<int>(mEigenVectors.rows());
    auto num_features = static_cast<int>(mEigenVectors.cols());
    cv::Mat data(num_elements,num_features,CV_32F);

    cv::eigen2cv(mEigenVectors,data);

    int clusterCount = num_clusters;
    cv::Mat labels;
    int attempts = 50;
    cv::Mat centers;
    kmeans(data, clusterCount, labels, cv::TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, cv::KMEANS_PP_CENTERS, centers );
    /*std::cout << "LABELS: " << std::endl;
    for (int i = 0; i < num_elements; i++) {
        std::cout << labels.at<int>(i) << std::endl;
    }*/

    return labels;
}