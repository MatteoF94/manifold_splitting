//
// Created by matteo on 10/08/18.
//

#include <cstdio>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <queue>
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
    mRoots = new std::vector<std::vector<MultiTreeNode*>*>();
    mFaceGroupId = new std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int>();
    MultiTreeNode* curr_node = last;
    std::vector<int>* group_ids = new std::vector<int>(num_elements,-1);

    // Going backward, propagate the node value and cut if necessary
    while (curr_node != nullptr) {

        // If there are already been k-1 cuts, the last one is mandatorily in the root, skipping the remaining nodes
        if(mNumGroups == mNumPartitions - 1) {
            cutTree(root,group_ids);
            std::vector<MultiTreeNode*>* curr_cut = new std::vector<MultiTreeNode*>();
            curr_cut->push_back(root);
            mRoots->push_back(curr_cut);
            break;
        }

        // Skip the node if it is invalid (already cut)
        if (!curr_node->valid_) {
            curr_node = curr_node->prev_;
            continue;
        }

        // Check for relatives, but only above a certain value
        if (curr_node->value_ >= double(mThreshold) * 0.33) {
            if(!curr_node->descendants_.empty()) {
                // If the search has been successful, a partition has been found and we can skip to the next item
                if (checkRelatives(curr_node, group_ids) == MultiTreePartitioner::CUT) {
                    curr_node = curr_node->prev_;
                    continue;
                }
            }
        }

        // Check if the future value of the parent will exceed the admissible threshold and cut preemptively
        if (curr_node->prev_ != nullptr) {

            // Check if the nodes are former siblings
            if (curr_node->prev_->parent_ == curr_node->parent_) {
                if (curr_node->prev_->value_ + curr_node->value_ > mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                    MultiTreeNode *max = curr_node->value_ > curr_node->prev_->value_ ? curr_node : curr_node->prev_;
                    cutTree(max, group_ids);
                    std::vector<MultiTreeNode*>* curr_cut = new std::vector<MultiTreeNode*>();
                    curr_cut->push_back(max);
                    mRoots->push_back(curr_cut);
                    mNumGroups++;

                    if(max != curr_node)
                        curr_node->parent_->value_ = curr_node->parent_->value_ + curr_node->value_;

                    curr_node = curr_node->prev_;

                    continue;
                }
            }
        }

        if(curr_node->parent_->parent_ == nullptr) {
            if (curr_node->parent_->mid_ == curr_node) {
                if (curr_node->prev_ != nullptr && curr_node->prev_->prev_ != nullptr) {

                    if (curr_node->prev_->prev_->value_ + curr_node->prev_->value_ + curr_node->value_ >
                        mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                        MultiTreeNode *max = curr_node->value_ > curr_node->prev_->value_ ? curr_node : curr_node->prev_;
                        max = max->value_ > curr_node->prev_->prev_->value_ ? max : curr_node->prev_->prev_;

                        cutTree(max, group_ids);
                        std::vector<MultiTreeNode*> *curr_cut = new std::vector<MultiTreeNode*>();
                        curr_cut->push_back(max);
                        mRoots->push_back(curr_cut);
                        mNumGroups++;

                        if (max != curr_node)
                            curr_node->parent_->value_ = curr_node->parent_->value_ + curr_node->value_;

                        curr_node = curr_node->prev_;

                        continue;
                    }
                }
            }
        }


        // If the current node is the root or exceeds the threshold, then cut
        if (curr_node->value_ >= mThreshold) {
            cutTree(curr_node, group_ids);
            std::vector<MultiTreeNode*>* curr_cut = new std::vector<MultiTreeNode*>();
            curr_cut->push_back(curr_node);
            mRoots->push_back(curr_cut);
            if (mNumGroups < mNumPartitions - 1) {
                mNumGroups++;
            }
        } else {
            curr_node->parent_->value_ = curr_node->parent_->value_ + curr_node->value_;
            curr_node->propagated_ = true;
        }

        curr_node = curr_node->prev_;
    }

    return *group_ids;
}

std::vector<int> MultiTreePartitioner::partitionThinByNumber(MultiTreeNode *last, MultiTreeNode* root, int num_elements) {
    mNumGroups = 0;
    MultiTreeNode* curr_node = last;
    std::vector<int>* group_ids = new std::vector<int>(num_elements,-1);
    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor, int> lane_map = createLanesMap(root);

    // Going backward, propagate the node value and cut if necessary
    while (curr_node != nullptr) {

        // If there are already been k-1 cuts, the last one is mandatorily in the root, skipping the remaining nodes
        if(mNumGroups == mNumPartitions - 1) {
            cutTree(root,group_ids);
            break;
        }

        // Skip the node if it is invalid (already cut)
        if (!curr_node->valid_) {
            curr_node = curr_node->prev_;
            continue;
        }

        // Check for relatives, but only above a certain value
        if (curr_node->value_ >= double(mThreshold) * 0.5) {

            // If the search has been successful, a partition has been found and we can skip to the next item
            if (checkRelativesThin(curr_node,lane_map,group_ids) == MultiTreePartitioner::CUT) {
                curr_node = curr_node->prev_;
                continue;
            }
        }

        // If the current node is the root or exceeds the threshold, then cut
        if (curr_node->value_ >= mThreshold) {
            cutTree(curr_node, group_ids);
            if (mNumGroups < mNumPartitions - 1) {
                mNumGroups++;
            }
        } else {
            curr_node->parent_->value_ = curr_node->parent_->value_ + curr_node->value_;
            curr_node->propagated_ = true;
        }

        curr_node = curr_node->prev_;
    }

    return *group_ids;
}

std::vector<int> MultiTreePartitioner::freePartitioning(MultiTreeNode *last, MultiTreeNode* root, int num_elements) {
    configParameters(10,1000,10,8);
    mNumGroups = 0;
    MultiTreeNode* curr_node = last;
    std::vector<int>* group_ids = new std::vector<int>(num_elements,-1);

    // Going backward, propagate the node value and cut if necessary
    while (curr_node != nullptr) {

        // Skip the node if it is invalid (already cut)
        if (!curr_node->valid_) {
            curr_node = curr_node->prev_;
            continue;
        }

        // Check for relatives, but only above a certain value
        if (curr_node->value_ >= double(mThreshold) * 0.4) {

            // If the search has been successful, a partition has been found and we can skip to the next item
            if (checkRelatives(curr_node, group_ids) == MultiTreePartitioner::CUT) {
                curr_node = curr_node->prev_;
                continue;
            }
        }

        // Check if the future value of the parent will exceed the admissible threshold and cut preemptively
        if (curr_node->prev_ != nullptr) {

            // Check if the nodes are former siblings
            if (curr_node->prev_->parent_ == curr_node->parent_) {
                if (curr_node->prev_->value_ + curr_node->value_ > mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                    MultiTreeNode *max = curr_node->value_ > curr_node->prev_->value_ ? curr_node : curr_node->prev_;
                    cutTree(max, group_ids);
                    mNumGroups++;

                    if(max != curr_node)
                        curr_node->parent_->value_ = curr_node->parent_->value_ + curr_node->value_;

                    curr_node = curr_node->prev_;

                    continue;
                }
            }
        }

        // If the current node is the root or exceeds the threshold, then cut
        if (curr_node->value_ >= mThreshold) {
            cutTree(curr_node, group_ids);
            mNumGroups++;
        } else {
            curr_node->parent_->value_ = curr_node->parent_->value_ + curr_node->value_;
            curr_node->propagated_ = true;
        }

        curr_node = curr_node->prev_;
    }

    return *group_ids;
}

MultiTreePartitioner::LinkageState MultiTreePartitioner::checkRelatives(MultiTreeNode* node, std::vector<int>* groups) {

    std::vector<MultiTreeNode*> nodes_to_cut;
    nodes_to_cut.push_back(node);

    int sum = node->value_;
    while (nodes_to_cut.size() < mDepth ) {
        MultiTreeNode* curr_node = nodes_to_cut.back();

        for (auto &relative : curr_node->descendants_) {
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
    sum = sum + node->value_;
    if (sum >= mThreshold && sum <= mThreshold*(1.0 + double(mEpsilon)/100.0)) {
        std::vector<MultiTreeNode*>* curr_cut = new std::vector<MultiTreeNode*>();
        for (auto &i : *stack) {
            propagateValueCut(i);
            cutTree(i, groups);
            curr_cut->push_back(i);
        }
        mRoots->push_back(curr_cut);
        if(mNumGroups < mNumPartitions - 1) {
            mNumGroups++;
        }
        return MultiTreePartitioner::CUT; // tree has been cut
    }

    while(stack->size() < mDepth) {
        for (auto &relative : node->descendants_) {
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

MultiTreePartitioner::LinkageState MultiTreePartitioner::checkDescendantsFull(MultiTreeNode *node, std::vector<int> *groups){
    BNode* root = new BNode;
    root->id = node->id_;
    std::vector<MultiTreeNode*>* ids = new std::vector<MultiTreeNode*>;
    ids->push_back(node);
    createMiniTree(root,node,ids,1);

    std::vector<BNode*>* trees = new std::vector<BNode*>;
    BNode* root_copy = new BNode;
    root_copy->id = node->id_;
    trees->push_back(root_copy);

    createTreePowerSet(root);
}

void MultiTreePartitioner::createMiniTree(MultiTreePartitioner::BNode* mini_node, MultiTreeNode* node, std::vector<MultiTreeNode*>* ids, int curr_depth){
    if(node->descendants_.empty() || curr_depth == 4)
        return;

    if(node->descendants_.size() == 1) {
        if(!isRelativeLinked(node->descendants_.at(0),ids)) {
            BNode *new_mini_node = new BNode;
            new_mini_node->id = node->descendants_.at(0)->id_;
            mini_node->left = new_mini_node;

            ids->push_back(node->descendants_.at(0));
            createMiniTree(new_mini_node, node->descendants_.at(0), ids, curr_depth + 1);
        }
    } else {
        if(!isRelativeLinked(node->descendants_.at(0),ids)) {
            BNode *new_mini_node_L = new BNode;
            new_mini_node_L->id = node->descendants_.at(0)->id_;
            mini_node->left = new_mini_node_L;

            ids->push_back(node->descendants_.at(0));
            createMiniTree(new_mini_node_L, node->descendants_.at(0), ids, curr_depth + 1);
        }

        if(!isRelativeLinked(node->descendants_.at(1),ids)) {
            BNode *new_mini_node_R = new BNode;
            new_mini_node_R->id = node->descendants_.at(1)->id_;
            mini_node->right = new_mini_node_R;

            ids->push_back(node->descendants_.at(1));
            createMiniTree(new_mini_node_R, node->descendants_.at(1), ids, curr_depth + 1);
        }
    }

}

std::vector<MultiTreePartitioner::BNode*> MultiTreePartitioner::createTreePowerSet(BNode* root) {

    std::queue<BNode*> node_queue;
    std::unordered_map<int,BNode*> node_map;
    node_queue.push(root);
    node_map.insert({root->id,root});

    while(!node_queue.empty()) {
        BNode* front_element = node_queue.front();
        if(front_element->left != nullptr) {
            node_queue.push(front_element->left);
            node_map.insert({front_element->left->id,front_element->left});
        }

        if(front_element->right != nullptr) {
            node_queue.push(front_element->right);
            node_map.insert({front_element->right->id,front_element->right});
        }
        node_queue.pop();
    }

    std::vector<std::vector<int>> trees;
    trees = searchSubTreeCombinations(root);

    for(auto tree : trees) {
        for(auto node : tree) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }

    // Select best rank
    std::vector<int> values;
    for(auto &tree : trees) {
        int curr_value = 0;
        for(auto &value : tree)
            curr_value = curr_value + node_map[value]->value;
        values.push_back(curr_value);
    }

    std::vector<int> max_positions;
    for (int i = 0; i < values.size(); ++i) {
        if (values.at(i) >= 20 && values.at(i) <= 25)
            max_positions.push_back(i);
    }
    std::cout << std::endl;
}

std::vector<std::vector<int>> MultiTreePartitioner::searchSubTreeCombinations(BNode* node) {
    std::vector<std::vector<int>> combinations;

    std::vector<std::vector<int>> from_left, from_right, cross_products;
    if(node->left != nullptr)
         from_left = searchSubTreeCombinations(node->left);

    if(node->right != nullptr)
        from_right = searchSubTreeCombinations(node->right);

    for (auto curr_left : from_left) {
        for (auto curr_right : from_right) {
            std::vector<int> curr_cross;
            curr_cross.reserve(curr_left.size() + curr_right.size());
            curr_cross.insert(curr_cross.end(), curr_left.begin(), curr_left.end());
            curr_cross.insert(curr_cross.end(), curr_right.begin(), curr_right.end());
            cross_products.push_back(curr_cross);
        }
    }

    combinations.reserve(from_left.size() + from_right.size() + cross_products.size());
    combinations.insert(combinations.end(), from_left.begin(), from_left.end());
    combinations.insert(combinations.end(), from_right.begin(), from_right.end());
    combinations.insert(combinations.end(), cross_products.begin(), cross_products.end());

    std::vector<int> tmp = {node->id};

    for(int i = 0; i < combinations.size(); ++i) {
        combinations.at(i).reserve(combinations.at(i).size() + tmp.size());
        combinations.at(i).insert(combinations.at(i).begin(), tmp.begin(), tmp.end());
    }

    combinations.push_back(tmp);

    return combinations;
}

MultiTreePartitioner::LinkageState MultiTreePartitioner::checkRelativesThin(MultiTreeNode* node,std::unordered_map<boost::graph_traits<Mesh>::face_descriptor, int> lane_map, std::vector<int>* groups) {

    int lane_first_chain = lane_map[node->id_];
    int sum = node->value_;

    //Check for 3-chains
    for (auto &relative_first : node->descendants_) {
        sum = sum + relative_first->value_;

        if (sum >= mThreshold && sum <= mThreshold*(1.0 + double(mEpsilon)/100.0)) {
            propagateValueCut(relative_first);
            cutTree(relative_first, groups);
            cutTree(node, groups);

            mNumGroups++;
            return MultiTreePartitioner::CUT; // tree has been cut

        } else {
            for (auto &relative_second: relative_first->descendants_) {

                if(lane_map[relative_second->id_] != lane_first_chain) {
                    sum = sum + relative_second->value_;

                    if (sum >= mThreshold && sum <= mThreshold * (1.0 + double(mEpsilon) / 100.0)) {
                        propagateValueCut(relative_first);
                        propagateValueCut(relative_second);
                        cutTree(relative_second, groups);
                        cutTree(relative_first, groups);
                        cutTree(node, groups);

                        mNumGroups++;
                        return MultiTreePartitioner::CUT; // tree has been cut

                    } else
                        sum = sum - relative_second->value_;
                }
            }

            sum = sum - relative_first->value_;
        }
    }

    if(node->descendants_.size() == 2) {
        MultiTreeNode* first = node->descendants_.at(0);
        MultiTreeNode* second = node->descendants_.at(1);

        if(lane_map[first->id_] != lane_map[second->id_]) {
            sum = sum + first->value_ + second->value_;
            if (sum >= mThreshold && sum <= mThreshold*(1.0 + double(mEpsilon)/100.0)) {
                propagateValueCut(first);
                propagateValueCut(second);
                cutTree(second, groups);
                cutTree(first, groups);
                cutTree(node, groups);

                mNumGroups++;
                return MultiTreePartitioner::CUT; // tree has been cut
            }
        }
    }

    return MultiTreePartitioner::UNSUCCESFUL;
}

bool MultiTreePartitioner::isRelativeLinked(MultiTreeNode *node, std::vector<MultiTreeNode*>* chain) {

    // TODO can be improved, doing only one sweep up to the highest element (the first for sure)
    for(int i = 0; i < chain->size()-1; i++) {
        MultiTreeNode* curr_ancestor = chain->at(i);
        MultiTreeNode* alt_node = node;
        if(alt_node == curr_ancestor)
            return true;

        while(alt_node->level_ >= curr_ancestor->level_) {
            if(alt_node->parent_ == curr_ancestor)
                return true;
            alt_node = alt_node->parent_;
        }
    }

    return false;
}

void MultiTreePartitioner::propagateValueCut(MultiTreeNode* node) {
    int value = node->value_;
    MultiTreeNode* sub_root = node;

    while(sub_root->propagated_) {
        sub_root->parent_->value_ = sub_root->parent_->value_ - value;
        sub_root = sub_root->parent_;
    }
}

void MultiTreePartitioner::cutTree (MultiTreeNode* sub_root, std::vector<int>* group_list) {
    if (sub_root == nullptr)
        return;

    if(sub_root->valid_) {
        sub_root->valid_ = false;
        group_list->at(sub_root->id_) = mNumGroups;
        //mFaceGroupId->insert({sub_root->id,mNumGroups});
        cutTree(sub_root->left_,group_list);
        cutTree(sub_root->right_,group_list);
        if(sub_root->parent_ == nullptr)
            cutTree(sub_root->mid_,group_list);
    }
}

std::unordered_map<boost::graph_traits<Mesh>::face_descriptor, int> MultiTreePartitioner::createLanesMap(MultiTreeNode* root) {

    std::unordered_map<boost::graph_traits<Mesh>::face_descriptor, int> lane_map;
    lane_map.insert({root->id_,0});

    if(root->left_ != nullptr) {
        MultiTreeNode* left_child = root->left_;

        while(left_child != nullptr) {
            lane_map.insert({left_child->id_,1});
            left_child = left_child->left_;
        }
    }

    if(root->right_ != nullptr) {
        MultiTreeNode* right_child = root->right_;

        while(right_child != nullptr) {
            lane_map.insert({right_child->id_,2});
            right_child = right_child->left_;
        }
    }

    if(root->mid_ != nullptr) {
        MultiTreeNode* mid_child = root->mid_;

        while(mid_child != nullptr) {
            lane_map.insert({mid_child->id_,3});
            mid_child = mid_child->left_;
        }
    }

    return lane_map;
};

std::vector<std::vector<MultiTreeNode*>*>* MultiTreePartitioner::getRoots() {
    /*for(int i = 0; i < mRoots->size(); ++i) {
        std::vector<MultiTreeNode*>* curr_vector = mRoots->at(i);
        for(int j = 0; j < curr_vector->size(); ++j) {
            MultiTreeNode* curr_node = curr_vector->at(j);
            std::cout << " " << std::endl;
        }
    }*/

    return mRoots;
}

std::unordered_map<boost::graph_traits<Mesh>::face_descriptor,int>* MultiTreePartitioner::getFaceGroupMap() {
    return mFaceGroupId;
}
