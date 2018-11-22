//
// Created by matteo on 21/11/18.
//

#ifndef MANIFOLD_SPLITTING_CREATORMANAGER_H
#define MANIFOLD_SPLITTING_CREATORMANAGER_H

#include "TreeTypes.h"
#include "Creator.h"
#include "AdoptionHandler.h"
#include "Concatenator.h"
#include "stopwatch.h"

class CreatorManager {
public:

    CreatorManager();
    ~CreatorManager();

    void setCreationMode(CreationMode creationMode);
    void configCreator(ConcatenationType concatenationType);
    void configAdoptionHandler(bool enableDeepAdoption, bool enableMultipleAdoption, int artifactDepth, int artifactWidth);
    void configConcatenator(ConcatenationType concatenationType);

    void createMultiTree(Mesh &mesh, Node *const &root);

private:

    std::unique_ptr<Creator> creator_;
    std::unique_ptr<AdoptionHandler> adoptionHandler_;
    std::unique_ptr<Concatenator> concatenator_;

    std::unique_ptr<Stopwatch> stopwatch_;

    CreationMode runtimeCreationMode_;
    bool adoptionEnabled_;
};


#endif //MANIFOLD_SPLITTING_CREATORMANAGER_H
