/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file Creator.h
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#ifndef MANIFOLD_SPLITTING_CREATOR_H
#define MANIFOLD_SPLITTING_CREATOR_H

#include <types.h>
#include "TreeTypes.h"

/**
 * @class Creator
 * @brief Creates the multi tree of a given mesh.
 * @details This class takes an input mesh and creates the corresponding multi tree. It can be also extended to convert a
 *          generic n-featured complex object (tetrahedra, graphs with features, hyper-volumes...).
 */
class Creator {
public:

    /**
     * @brief Class constructor.
     */
    Creator();

    /**
     * @brief Class destructor.
     */
    virtual ~Creator();

    /**
     * @brief Set the tree concatenation type between nodes.
     * @param concatenationType
     */
    void configCreator(ConcatenationType concatenationType);

    /**
     * @brief Selects which algorithm variant is used to build the tree, depending on the way the nodes should be linked.
     * @param mesh The mesh to be converted.
     * @param root Root of the tree representing the mesh.
     */
    void buildTree(const Mesh &mesh, Node *const &root);

protected:

    /**
     * @brief Builds the tree expanding neighbouring nodes left to right breadth first.
     * @param mesh The mesh to be converted.
     * @param root The root of the tree representing the mesh.
     */
    virtual void buildTreeLtR(const Mesh &mesh, Node *const &root) = 0;

    /**
     * @brief Builds the tree expanding neighbouring nodes right to left breadth first.
     * @param mesh The mesh to be converted.
     * @param root The root of the tree representing the mesh.
     */
    virtual void buildTreeRtL(const Mesh &mesh, Node *const &root) = 0;

    /**
     * @brief Builds the tree expanding neighbouring nodes alternating left to right and right to left  breadth first
     *        at each tree level.
     * @param mesh The mesh to be converted.
     * @param root The root of the tree representing the mesh.
     */
    virtual void buildTreeBalanced(const Mesh &mesh, Node *const &root) = 0;

    /**
     * @brief Builds the tree expanding neighbouring nodes alternating left to right and right to left  breadth first
     *        at each node expansion.
     * @param mesh The mesh to be converted.
     * @param root The root of the tree representing the mesh.
     */
    virtual void buildTreeFlipped(const Mesh &mesh, Node *const &root) = 0;

    /**
     * @brief Set one of the current node descendant or relative, depending on the position.
     * @param node Current node in the tree.
     * @param neighbour Index of the already inserted tree.
     * @param treeNodes List of all the nodes inserted in the tree (some also not inserted).
     */
    void insertNodeDescAndRels(Node *const &node, const CGAL::Face_around_face_iterator<Mesh> &neighbour, const std::vector<Node*> &treeNodes);

    ConcatenationType concatenationType_; /** Indicates the way nodes are linked through prev and next. */
};

#endif //MANIFOLD_SPLITTING_CREATOR_H
