//
// Created by matteo on 22/11/18.
//

#ifndef MANIFOLD_SPLITTING_PARALLELCREATOR_H
#define MANIFOLD_SPLITTING_PARALLELCREATOR_H

#include <types.h>
#include "Creator.h"

class ParallelCreator : public Creator {
public:

    ParallelCreator();
protected:

    /**
     * Overrides base class function.
     */
    void buildTreeLtR(const Mesh &mesh, Node *const &root) override;

    /**
     * Overrides base class function.
     */
    void buildTreeRtL(const Mesh &mesh, Node *const &root) override;

    /**
     * Overrides base class function.
     */
    void buildTreeBalanced(const Mesh &mesh, Node *const &root) override;

    /**
     * Overrides base class function.
     */
    void buildTreeFlipped(const Mesh &mesh, Node *const &root) override;

    void buildTreeTop(const Mesh &mesh,
                      Node *const &root,
                      std::vector<Node *> &leaves,
                      std::vector<Node *> &treeNodes,
                      std::vector<bool> &isNodeInserted,
                      std::vector<std::vector<CGAL::SM_Face_index>> &descIds);

    void insertNodeDescFromIds(std::vector<Node *> &nodes, std::vector<std::vector<CGAL::SM_Face_index>> &ids);

private:

    unsigned int maxLevelToParallelize_;
};


#endif //MANIFOLD_SPLITTING_PARALLELCREATOR_H
