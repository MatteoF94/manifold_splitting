/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file AdoptionHandler.h
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#ifndef MANIFOLD_SPLITTING_ADOPTIONHANDLER_H
#define MANIFOLD_SPLITTING_ADOPTIONHANDLER_H

#include <types.h>
#include "TreeTypes.h"

/**
 * @class AdoptionHandler
 * @brief Handles the adoption of branches in the tree.
 * @details To avoid artifacts, such as small chains of nodes (branches with very low children span), this class
 *          transfers branches along the tree, towards a complete tree.
 */
class AdoptionHandler {
public:
    /**
     * @brief Class constructor.
     */
    AdoptionHandler();

    /**
     * @brief Class destructor.
     */
    ~AdoptionHandler();

    /**
     * @brieft Sets the parameters for the adoption algorithm.
     * @param enableDeepAdoption True if adopted nodes can adopt other nodes, false otherwise.
     * @param enableMultipleAdoption True if one node can be adopted multiple times, false otherwise.
     * @param artifactDepth Maximum depth of an artifact
     * @param artifactWidth Maximum width of an artifact (number of nodes for each level of the branch considered).
     * @return Void.
     */
    void configHandler(bool enableDeepAdoption, bool enableMultipleAdoption, int artifactDepth, int artifactWidth);

    /**
     * @brief Goes through the tree and moves branches in the tree, making nodes adopt them.
     * @param root The root of the tree for which adoption is requested.
     * @return Void.
     */
    void adoptBranches(Node *const &root);

private:

    /**
     * @brief Checks if a portion of a tree branch is an artifact
     * @param node
     * @return
     */
    bool isArtifact(const Node *const &node);

    void adjustDescAndRels(Node *const &node);

    bool deepAdoptionEnabled_; /** Whether an adopted node can itself adopt another node. */
    bool multiAdoptionEnabled_; /** Whether multiple adoptions of the same node is allowed or not. */
    int artifactDepth_; /** Artifact maximum depth. */
    int artifactWidth_; /** Artifact maximum width. */
};


#endif //MANIFOLD_SPLITTING_ADOPTIONHANDLER_H
