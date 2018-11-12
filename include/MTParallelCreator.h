//
// Created by matteo on 07/11/18.
//

#ifndef MANIFOLD_SPLITTING_MTPARALLELCREATOR_H
#define MANIFOLD_SPLITTING_MTPARALLELCREATOR_H

#include <omp.h>

#include <types.h>
#include <MultiTreeManager.h>

typedef boost::graph_traits<FiniteDual>::vertex_descriptor DualVertexDescriptor;

class MTParallelCreator {
public:
    MTParallelCreator(MultiTreeManager *mt_manager) : descendant_max_gap_(100),
                                                      branch_chaining_type_(MultiTreeManager::ChainingType::FLIP),
                                                      tree_chaining_type_(MultiTreeManager::ChainingType::BALANCED){
            tree_manager_ = mt_manager;
    }

    void configCreation(MultiTreeManager::ChainingType branch_chaining, MultiTreeManager::ChainingType tree_chaining) {
        branch_chaining_type_ = branch_chaining;
        tree_chaining_type_ = tree_chaining;
    }

    MultiTreeNode* createParallelTree(FiniteDual dual);

private:
    void createParallelTreeBodyLTR(MultiTreeNode *front_element, FiniteDual dual, omp_lock_t insertion_lock, std::vector<bool>* inserted_map, std::vector<MultiTreeNode*>* node_real, std::vector<std::vector<DualVertexDescriptor>>* relatives_id);
    void createParallelTreeBodyRTL(MultiTreeNode *front_element, FiniteDual dual, omp_lock_t insertion_lock, std::vector<bool>* inserted_map, std::vector<MultiTreeNode*>* node_real, std::vector<std::vector<DualVertexDescriptor>>* relatives_id);
    void createParallelTreeBodyBAL(MultiTreeNode *front_element, FiniteDual dual, omp_lock_t insertion_lock, std::vector<bool>* inserted_map, std::vector<MultiTreeNode*>* node_real, std::vector<std::vector<DualVertexDescriptor>>* relatives_id);
    void createParallelTreeBodyFLIP(MultiTreeNode *front_element, FiniteDual dual, omp_lock_t insertion_lock, std::vector<bool>* inserted_map, std::vector<MultiTreeNode*>* node_real, std::vector<std::vector<DualVertexDescriptor>>* relatives_id);

    void chainParallelTreeLTR(MultiTreeNode *root);
    void chainParallelTreeRTL(MultiTreeNode *root);
    void chainParallelTreeBAL(MultiTreeNode *root);
    void chainParallelTreeFLIP(MultiTreeNode* root);


    /*---- Configuration variables ----*/
    int descendant_max_gap_;
    MultiTreeManager::ChainingType branch_chaining_type_;
    MultiTreeManager::ChainingType tree_chaining_type_;

    MultiTreeManager* tree_manager_;
};


#endif //MANIFOLD_SPLITTING_MTPARALLELCREATOR_H
