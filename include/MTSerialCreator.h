//
// Created by matteo on 26/10/18.
//

#ifndef MANIFOLD_SPLITTING_MTSERIALCREATOR_H
#define MANIFOLD_SPLITTING_MTSERIALCREATOR_H

#include <types.h>
#include <MultiTreeManager.h>

class MTSerialCreator {
public:
    MTSerialCreator(MultiTreeManager *mt_manager) : chaining_type_(MultiTreeManager::ChainingType::LTR){
        tree_manager_ = mt_manager;
    }

    void configCreation(MultiTreeManager::ChainingType chaining) {
        chaining_type_ = chaining;
    }

    MultiTreeNode* createSerialTree(FiniteDual dual);

private:
    MultiTreeNode* createSerialTreeLTR(FiniteDual dual);
    MultiTreeNode* createSerialTreeRTL(FiniteDual dual);
    MultiTreeNode* createSerialTreeBAL(FiniteDual dual);
    MultiTreeNode* createSerialTreeFLIP(FiniteDual dual);
    MultiTreeNode* createSerialTreeDF(FiniteDual dual);

    void trimSerialTree(MultiTreeNode* root);
    void adjustDescendants(MultiTreeNode* node);
    bool isChain(MultiTreeNode* node);

    /*---- Configuration variables ----*/
    MultiTreeManager::ChainingType chaining_type_;

    MultiTreeManager* tree_manager_;
};


#endif //MANIFOLD_SPLITTING_MTSERIALCREATOR_H
