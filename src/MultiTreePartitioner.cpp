//
// Created by matteo on 10/08/18.
//

#include <cstdio>
#include <iostream>
#include <stack>
#include "MultiTreePartitioner.h"

MultiTreePartitioner::MultiTreePartitioner() : mDepth(0), mThreshold(0), mEpsilon(10), mNumPartitions(1){}

void MultiTreePartitioner::configParameters(int search_depth, int threshold, int epsilon, int num_partitions) {
    mDepth = search_depth;
    mThreshold = threshold;
    mNumPartitions = num_partitions;

    if(epsilon >= 0 && epsilon <= 100)
        mEpsilon = epsilon;
    else
        std::cerr << "Epsilon should be in range [0,100], using default (10)." << std::endl;
}

std::vector<int> MultiTreePartitioner::partitionByNumber(MultiTreeNode *last, MultiTreeNode* root, int num_elements) {
    mNumGroups = 0;
    MultiTreeNode* curr_node = last;
    std::vector<int>* group_ids = new std::vector<int>(num_elements,-1);

    // Going backward, propagate the node value and cut if necessary
    while (curr_node != nullptr) {

        // If there are already been k-1 cuts, the last one is mandatorily in the root, skipping the remaining nodes
        if(mNumGroups == mNumPartitions - 1) {
            cutTree(root,group_ids);
            break;
        }

        // Skip the node if it is invalid (already cut)
        if (!curr_node->valid) {
            curr_node = curr_node->prev;
            continue;
        }

        // Check for relatives, but only above a certain value
        if (curr_node->value >= mThreshold / 3) {

            // If the search has been successful, a partition has been found and we can skip to the next item
            if (checkRelatives(curr_node, group_ids) == MultiTreePartitioner::CUT) {
                curr_node = curr_node->prev;
                continue;
            }
        }

        // Check if the future value of the parent will exceed the admissible threshold and cut preemptively
        if (curr_node->prev != nullptr) {

            // Check if the nodes are former siblings
            if (curr_node->prev->parent == curr_node->parent) {
                if (curr_node->prev->value + curr_node->value > mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                    MultiTreeNode *max = curr_node->value > curr_node->prev->value ? curr_node : curr_node->prev;
                    cutTree(max, group_ids);
                    mNumGroups++;

                    if(max != curr_node)
                        curr_node->parent->value = curr_node->parent->value + curr_node->value;

                    curr_node = curr_node->prev;

                    continue;
                }
            }
        }


        // If the current node is the root or exceeds the threshold, then cut
        if (curr_node->value >= mThreshold) {
            cutTree(curr_node, group_ids);
            if (mNumGroups < mNumPartitions - 1) {
                mNumGroups++;
            }
        } else {
            curr_node->parent->value = curr_node->parent->value + curr_node->value;
            curr_node->propagated = true;
        }

        curr_node = curr_node->prev;
    }

    curr_node = root;
    /*while(curr_node->next != nullptr) {
        if (group_ids->at(curr_node->id) == -1)
            std::cout << "cazzo?" << std::endl;
        curr_node = curr_node->next;
    }*/
    return *group_ids;
}

MultiTreePartitioner::LinkageState MultiTreePartitioner::checkRelatives(MultiTreeNode* node, std::vector<int>* groups) {

    std::vector<MultiTreeNode*> nodes_to_cut;
    nodes_to_cut.push_back(node);

    int sum = node->value;
    while (nodes_to_cut.size() < mDepth ) {
        MultiTreeNode* curr_node = nodes_to_cut.back();

        for (auto &relative : curr_node->relatives) {
            MultiTreePartitioner::LinkageState state = checkRelatives(relative,&nodes_to_cut, sum, groups);
            if(state == MultiTreePartitioner::CUT)
                return MultiTreePartitioner::CUT;
        }
        break;
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
        if(mNumGroups < mNumPartitions - 1) {
            mNumGroups++;
        }
        return MultiTreePartitioner::CUT; // tree has been cut
    }

    while(stack->size() < mDepth) {
        for (auto &relative : node->relatives) {
            if(!isRelativeLinked(relative,stack)) {
                MultiTreePartitioner::LinkageState state = checkRelatives(relative,stack,sum,groups);
                if(state == MultiTreePartitioner::CUT)
                    return MultiTreePartitioner::CUT;
            }
        }
        break;
    }

    stack->pop_back();
    return MultiTreePartitioner::UNSUCCESFUL;
}

bool MultiTreePartitioner::isRelativeLinked(MultiTreeNode *node, std::vector<MultiTreeNode*>* chain) {

    // TODO can be improved, doing only one sweep up to the highest element (the first for sure)
    for(int i = 0; i < chain->size()-1; i++) {
        MultiTreeNode* curr_ancestor = chain->at(i);
        if(node == curr_ancestor)
            return true;

        while(node->level < curr_ancestor->level) {
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
    if (sub_root == nullptr)
        return;

    if(sub_root->valid) {
        sub_root->valid = false;
        group_list->at(sub_root->id) = mNumGroups;
        cutTree(sub_root->left,group_list);
        cutTree(sub_root->right,group_list);
        if(sub_root->parent == nullptr)
            cutTree(sub_root->mid,group_list);
    }
}