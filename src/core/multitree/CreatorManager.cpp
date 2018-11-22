//
// Created by matteo on 21/11/18.
//

#include "CreatorManager.h"
#include <memory>
#include "SerialCreator.h"
#include <spdlog/spdlog.h>

CreatorManager::CreatorManager()  : creator_(new SerialCreator),
                                    adoptionHandler_(new AdoptionHandler),
                                    concatenator_(new Concatenator),
                                    stopwatch_(new Stopwatch),
                                    runtimeCreationMode_(CreationMode::Serial),
                                    adoptionEnabled_(true)
{
}

CreatorManager::~CreatorManager()
{
}

void CreatorManager::setCreationMode(CreationMode creationMode)
{
    switch (creationMode)
    {
        case CreationMode::Serial :
            creator_ = std::make_unique<SerialCreator>();
            runtimeCreationMode_ = CreationMode::Serial;
            break;
        case CreationMode::Parallel :
            //creator_.reset(new ParallelCreator);
            runtimeCreationMode_ = CreationMode::Parallel;
            break;
    }
}

void CreatorManager::configCreator(ConcatenationType concatenationType)
{
    creator_->configCreator(concatenationType);
}

void CreatorManager::configAdoptionHandler(bool enableDeepAdoption, bool enableMultipleAdoption, int artifactDepth,
                                           int artifactWidth)
{
    adoptionHandler_->configHandler(enableDeepAdoption,enableMultipleAdoption,artifactDepth,artifactWidth);
}

void CreatorManager::configConcatenator(ConcatenationType concatenationType)
{
    concatenator_->setConcatenationType(concatenationType);
}

void CreatorManager::createMultiTree(Mesh &mesh, Node *const &root)
{
    double elapsedTime = 0.0;

    //TODO implement a way to select a custom initial node
    boost::graph_traits<FiniteDual>::vertex_descriptor initDsc = *mesh.faces_begin();
    root->id_ = initDsc;

    spdlog::info("CreatorManager::createMultiTree ---- building tree");
    stopwatch_->start();
    creator_->buildTree(mesh,root);
    elapsedTime = stopwatch_->stop();
    spdlog::info("CreatorManager::createMultiTree ---- finished building tree in {} seconds", elapsedTime);

    if(adoptionEnabled_)
    {
        spdlog::info("CreatorManager::createMultiTree ---- adopting branches");
        stopwatch_->start();
        adoptionHandler_->adoptBranches(root);
        elapsedTime = stopwatch_->stop();
        spdlog::info("CreatorManager::createMultiTree ---- finished adopting branches in {} seconds", elapsedTime);

        spdlog::info("CreatorManager::createMultiTree ---- concatenating nodes");
        stopwatch_->start();
        concatenator_->concatenateTree(root);
        elapsedTime = stopwatch_->stop();
        spdlog::info("CreatorManager::createMultiTree ---- finished concatenating nodes in {} seconds", elapsedTime);
    }
    else if(runtimeCreationMode_ == CreationMode::Parallel)
    {
        spdlog::info("CreatorManager::createMultiTree ---- concatenating nodes");
        stopwatch_->start();
        concatenator_->concatenateTree(root);
        elapsedTime = stopwatch_->stop();
        spdlog::info("CreatorManager::createMultiTree ---- finished concatenating nodes in {} seconds", elapsedTime);
    }
}
