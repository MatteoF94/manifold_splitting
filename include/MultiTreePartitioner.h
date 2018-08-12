//
// Created by matteo on 10/08/18.
//

#ifndef MANIFOLD_SPLITTING_MULTITREEPARTITIONER_H
#define MANIFOLD_SPLITTING_MULTITREEPARTITIONER_H

#include <types.h>

class MultiTreePartitioner {
public:
    MultiTreePartitioner();

    std::vector<int> partitionByNumber(MultiTreeNode* last);
    void configParameters(int search_depth, int threshold, int epsilon);

private:
    enum LinkageState {CUT,UNSUCCESFUL};

    void propagateValueCut(MultiTreeNode* sub_root);
    void cutTree(MultiTreeNode* sub_root, std::vector<int>* group_list);
    LinkageState checkRelatives(MultiTreeNode* node, std::vector<int>* groups);
    LinkageState checkRelatives(MultiTreeNode* node, std::vector<MultiTreeNode*>* stack, int curr_depth, std::vector<int>* groups);
    bool isRelativeLinked(MultiTreeNode* node, std::vector<MultiTreeNode*>* chain);
    int mNumGroups;

    // Config variables
    int mDepth;
    int mThreshold;
    int mEpsilon;


};


#endif //MANIFOLD_SPLITTING_MULTITREEPARTITIONER_H