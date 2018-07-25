//
// Created by matteo on 24/07/18.
//

#ifndef MANIFOLD_SPLITTING_SPECTRALCLUSTERING_H
#define MANIFOLD_SPLITTING_SPECTRALCLUSTERING_H

#include <vector>
#include <eigen3/Eigen/Core>
#include <opencv2/core.hpp>

class SpectralClustering {
public:
    SpectralClustering(Eigen::MatrixXf& similarity_matrix, int num_eigenvectors);
    virtual ~SpectralClustering();

    void setupEigenvectors();
    cv::Mat clusterKmeans(int num_clusters);

private:
    int mNumEigenVectors;
    Eigen::MatrixXf mEigenVectors;
    Eigen::MatrixXf mSimilarityMat;
    int mNumClusters;
};


#endif //MANIFOLD_SPLITTING_SPECTRALCLUSTERING_H
