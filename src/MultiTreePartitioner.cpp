//
// Created by matteo on 10/08/18.
//

#include <cstdio>
#include <iostream>
#include <stack>
#include "MultiTreePartitioner.h"

MultiTreePartitioner::MultiTreePartitioner() : mDepth(0), mThreshold(0), mEpsilon(10){}

void MultiTreePartitioner::configParameters(int search_depth, int threshold, int epsilon) {
    mDepth = search_depth;
    mThreshold = threshold;

    if(epsilon >= 0 && epsilon <= 100)
        mEpsilon = epsilon;
    else
        std::cerr << "Epsilon should be in range [0,100], using default (10)." << std::endl;
}

std::vector<int> MultiTreePartitioner::partitionByNumber(MultiTreeNode *last) {

    MultiTreeNode* curr_node = last;
    MultiTreeNode* mi;
    std::vector<int>* miao = new std::vector<int>(69666);

    while (curr_node != nullptr) {

        // Check for sibling possible over thresholding
        /*for (int i = 0; i < curr_node->siblings.size(); i++) {
            if (curr_node->value + curr_node->siblings.at(i)->value >= mThreshold) {
                cutTree(curr_node,miao);
                mNumGroups++;
                mode = 1;
                break;
            }
        }*/

        // Check for relatives, but only above a certain value
        if (curr_node->value >= mThreshold / 3) {
            // If the search has been successful, a partition has been found and we can skip to the next item
            if (checkRelatives(curr_node, miao) == MultiTreePartitioner::CUT) {
                curr_node = curr_node->prev;
                continue;
            }
        }

        // Check if the future value of the parent will exceed the admissible threshold and cut preemptively
        if (curr_node->next != nullptr) {
            // Check if the nodes are former siblings
            if (curr_node->next->parent == curr_node->parent) {
                if (curr_node->next->value + curr_node->value > mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                    MultiTreeNode *max = curr_node->value > curr_node->next->value ? curr_node : curr_node->next;
                    cutTree(max, miao);
                    curr_node = curr_node->prev;
                    continue;
                }
            }
        }

        if (curr_node->prev != nullptr) {
            // Check if the nodes are former siblings
            if (curr_node->prev->parent == curr_node->parent) {
                if (curr_node->prev->value + curr_node->value > mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                    MultiTreeNode *max = curr_node->value > curr_node->prev->value ? curr_node : curr_node->prev;
                    cutTree(max, miao);
                    curr_node = curr_node->prev;
                    continue;
                }
            }
        }


        // If the current node is the root or exceeds the threshold, then cut
        if (curr_node->value >= mThreshold || curr_node->parent == nullptr) {
            cutTree(curr_node, miao);
            mNumGroups++;
        } else {
            curr_node->parent->value = curr_node->parent->value + curr_node->value;
            curr_node->propagated = true;
        }

        if(curr_node->prev == nullptr)
            mi = curr_node;

        curr_node = curr_node->prev;
    }

    // Curr node is now root
    int i = 0;
    curr_node = mi;
    while (curr_node != nullptr) {
        //std::cout << "Node " << curr_node->id << ", #children " << curr_node->value-1 << std::endl;
        curr_node = curr_node->next;
        i++;
    }

    int zeros = 0;
    int ones = 0;
    for (int j = 0; j < miao->size(); j++) {
        if(miao->at(j)) ones++;
        else zeros++;
        //std::cout << miao->at(j) << std::endl;
    }

    std::cout << "C0: " << zeros << std::endl;
    std::cout << "C1: " << ones << std::endl;
    return *miao;
}

MultiTreePartitioner::LinkageState MultiTreePartitioner::checkRelatives(MultiTreeNode* node, std::vector<int>* groups) {

    std::vector<MultiTreeNode*> nodes_to_cut;
    nodes_to_cut.push_back(node);

    int sum = node->value;
    while (nodes_to_cut.size() < mDepth) {
        MultiTreeNode* curr_node = nodes_to_cut.back();

        for (auto &relative : curr_node->relatives) {
            if(checkRelatives(relative,&nodes_to_cut, sum, groups) == MultiTreePartitioner::CUT)
                return MultiTreePartitioner::CUT;
        }
    }

    return MultiTreePartitioner::UNSUCCESFUL;
}

MultiTreePartitioner::LinkageState MultiTreePartitioner::checkRelatives(MultiTreeNode* node, std::vector<MultiTreeNode*>* stack, int sum, std::vector<int>* groups) {
    stack->push_back(node);
    sum = sum + node->value;
    if (sum >= mThreshold && sum <= mThreshold*(1.0 + double(mEpsilon)/100.0)) {
        for (auto &i : *stack) {
            propagateValueCut(i);
            cutTree(i, groups);
        }
        mNumGroups++;
        return MultiTreePartitioner::CUT; // tree has been cut
    }

    while(stack->size() < mDepth) {
        for (auto &relative : node->relatives) {
            if(!isRelativeLinked(relative,stack)) {
                if(checkRelatives(relative, stack, sum, groups) == MultiTreePartitioner::CUT);
                    return MultiTreePartitioner::CUT;
            }
        }
    }

    stack->pop_back();
    return MultiTreePartitioner::UNSUCCESFUL;
}

bool MultiTreePartitioner::isRelativeLinked(MultiTreeNode *node, std::vector<MultiTreeNode*>* chain) {

    // TODO can be improved, doing only one sweep up to the highest element (the first for sure)
    for(int i = 0; i < chain->size()-1; i++) {
        MultiTreeNode* curr_ancestor = chain->at(i);

        while(node->level <= curr_ancestor->level) {
            if(node->parent == curr_ancestor)
                return true;
            node = node->parent;
        }
    }

    return false;
}

void MultiTreePartitioner::propagateValueCut(MultiTreeNode* sub_root) {
    int value = sub_root->value;

    while(sub_root->propagated) {
        sub_root->parent->value = sub_root->parent->value - value;
        sub_root = sub_root->parent;
    }
}

void MultiTreePartitioner::cutTree (MultiTreeNode* sub_root, std::vector<int>* group_list) {
    sub_root->valid = false;
    if(sub_root != nullptr && sub_root->valid) {
        group_list->at(sub_root->id) = mNumGroups;
        cutTree(sub_root->left,group_list);
        cutTree(sub_root->right,group_list);
    }
}