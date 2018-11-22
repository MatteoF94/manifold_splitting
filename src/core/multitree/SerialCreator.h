/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file SerialCreator.h
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#ifndef MANIFOLD_SPLITTING_SERIALCREATOR_H
#define MANIFOLD_SPLITTING_SERIALCREATOR_H

#include <types.h>
#include "Creator.h"

/**
 * @class SerialCreator
 * @extends Creator
 * @brief Creates the multi tree of a given mesh with serial algorithms.
 * @details This class takes an input mesh and creates the corresponding multi tree using serial algorithms. It can
 *          be also extended to convert a generic n-featured complex object (tetrahedra, graphs with features, hyper-volumes...).
 */
class SerialCreator : public Creator {
public:

    /**
     * @brief Class constructor.
     */
    SerialCreator();

    /**
     * @brief Class destructor.
     */
    ~SerialCreator() override;

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

    /**
     * @brief Set one of the current node descendant or relative, depending on the position.
     * @param node Current node in the tree.
     * @param neighbour Index of the already inserted tree.
     * @param treeNodes List of all the nodes inserted in the tree (some also not inserted).
     */
    void insertNodeDescAndRels(Node *const &node, const CGAL::Face_around_face_iterator<Mesh> &neighbour, const std::vector<Node*> &treeNodes);
};

#endif //MANIFOLD_SPLITTING_SERIALCREATOR_H
