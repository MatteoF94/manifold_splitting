//
// Created by matteo on 24/11/18.
//

#ifndef MANIFOLD_SPLITTING_UTILITIES_H
#define MANIFOLD_SPLITTING_UTILITIES_H


#include "TreeTypes.h"

class Utilities {
public:

    bool checkTreeIntegrity(const Node *const &root, unsigned int numNodes);

private:

    bool checkTreeConcatenation(const Node *const &root, unsigned int numNodes);
    bool checkTreeStructure(const Node *const &root, unsigned int numNodes);
    bool checkTreeSemantic(const Node *const &root);
};


#endif //MANIFOLD_SPLITTING_UTILITIES_H
