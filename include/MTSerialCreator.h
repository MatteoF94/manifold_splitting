//
// Created by matteo on 26/10/18.
//

#ifndef MANIFOLD_SPLITTING_MTSERIALCREATOR_H
#define MANIFOLD_SPLITTING_MTSERIALCREATOR_H

#include <types.h>
#include <MultiTreeManager.h>

class MTSerialCreator {
public:
    MTSerialCreator(MultiTreeManager *mt_manager) : chaining_type_(MultiTreeManager::ChainingType::LTR), with_adoptions_(false){
        tree_manager_ = mt_manager;
    }

    void configCreation(MultiTreeManager::ChainingType chaining, bool with_adoptions) {
        chaining_type_ = chaining;
        with_adoptions_ = with_adoptions;
    }

    MultiTreeNode* createSerialTree(FiniteDual dual);
    std::vector<int> createSerialTreeDF(FiniteDual dual,Mesh mesh);

private:
    MultiTreeNode* createSerialTreeLTR(FiniteDual dual);
    MultiTreeNode* createSerialTreeRTL(FiniteDual dual);
    MultiTreeNode* createSerialTreeBAL(FiniteDual dual);
    MultiTreeNode* createSerialTreeFLIP(FiniteDual dual);


    void trimSerialTree(MultiTreeNode* root);
    void adjustDescendants(MultiTreeNode* node);
    bool isChain(MultiTreeNode* node);

    /*---- Configuration variables ----*/
    MultiTreeManager::ChainingType chaining_type_;
    bool with_adoptions_;

    MultiTreeManager* tree_manager_;
};


#endif //MANIFOLD_SPLITTING_MTSERIALCREATOR_H
