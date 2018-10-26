//
// Created by matteo on 26/10/18.
//

#ifndef MANIFOLD_SPLITTING_MTSERIALCREATOR_H
#define MANIFOLD_SPLITTING_MTSERIALCREATOR_H

#include <types.h>
#include <MultiTreeManager.h>

class MTSerialCreator {
public:
    MTSerialCreator() {}

    MultiTreeNode* createSerialTree(FiniteDual dual, MultiTreeManager::CreationMode::Type creation_mode, int max_depth);
    void setCreationMode(MultiTreeManager::CreationMode creation_mode);

private:
    MultiTreeNode* createSerialTreeLTR(FiniteDual dual, int max_depth);
    MultiTreeNode* createSerialTreeRTL(FiniteDual dual, int max_depth);
    MultiTreeNode* createSerialTreeBAL(FiniteDual dual, int max_depth);
    MultiTreeNode* createSerialTreeHF(FiniteDual dual, int max_depth);
};


#endif //MANIFOLD_SPLITTING_MTSERIALCREATOR_H
