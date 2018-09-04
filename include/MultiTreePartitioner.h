//
// Created by matteo on 10/08/18.
//

#ifndef MANIFOLD_SPLITTING_MULTITREEPARTITIONER_H
#define MANIFOLD_SPLITTING_MULTITREEPARTITIONER_H

#include <types.h>
#include <unordered_map>

class MultiTreePartitioner {
public:
    struct BNode {
        BNode* left;
        BNode* right;
        int value;
        int id;

        BNode() :left(nullptr), right(nullptr) {}
    };

    MultiTreePartitioner();

    std::vector<int> partitionByNumber(MultiTreeNode* last, MultiTreeNode* root, int num_elements);
    std::vector<int> partitionThinByNumber(MultiTreeNode *last, MultiTreeNode* root, int num_elements);
    std::vector<int> freePartitioning(MultiTreeNode *last, MultiTreeNode* root, int num_elements);

    void configParameters(int search_depth, int threshold, int epsilon, int num_partitions);
    std::vector<MultiTreePartitioner::BNode*> createTreePowerSet(BNode* root);
    void createLeftPowerTree(BNode* root, BNode* root_copy, int depth);

private:
    enum LinkageState {CUT,UNSUCCESFUL};

    void propagateValueCut(MultiTreeNode* sub_root);
    void cutTree(MultiTreeNode* sub_root, std::vector<int>* group_list);
    LinkageState checkRelatives(MultiTreeNode* node, std::vector<int>* groups);
    LinkageState checkRelatives(MultiTreeNode* node, std::vector<MultiTreeNode*>* stack, int curr_depth, std::vector<int>* groups);
    LinkageState checkDescendantsFull(MultiTreeNode* node, std::vector<int>* groups);
    LinkageState checkRelativesThin(MultiTreeNode* node,std::unordered_map<boost::graph_traits<Mesh>::face_descriptor, int> lane_map, std::vector<int>* groups);
    bool isRelativeLinked(MultiTreeNode* node, std::vector<MultiTreeNode*>* chain);
    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor, int> createLanesMap(MultiTreeNode* root);
    void createMiniTree(BNode* mini_node, MultiTreeNode* node, std::vector<MultiTreeNode*>* ids, int curr_depth);
    std::vector<std::vector<int>> searchSubTreeCombinations(BNode* node);


    int mNumGroups;

    // Config variables
    int mDepth;
    int mThreshold;
    int mEpsilon;
    int mNumPartitions;


};


#endif //MANIFOLD_SPLITTING_MULTITREEPARTITIONER_H
