/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file Concatenator.h
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#ifndef MANIFOLD_SPLITTING_CONCATENATOR_H
#define MANIFOLD_SPLITTING_CONCATENATOR_H

#include "TreeTypes.h"

/**
 * @class Concatenator
 * @brief Concatenates the nodes of a multi tree.
 * @details This class concatenates the nodes of a multi tree in different ways, all breadth first:
 *          left to right, right to left, alternated (also called balanced), and flipped (each node is
 *          alternatively connected left to right and right to left to the next).
 */
class Concatenator {
public:

    /**
     * @brief Class constructor.
     */
    Concatenator();

    /**
     * @brief Class destructor.
     */
    ~Concatenator();

    /**
     * @brief Set the concatenation type.
     * @param concatenationType The concatenation type.
     * @return Void.
     */
    void setConcatenationType(const ConcatenationType& concatenationType);

    /**
     * @brief Concatenates the nodes of a tree, setting prev and next for each.
     * @param root The root of the tree.
     * @return Void.
     */
    void concatenateTree(Node *const &root);

private:
    /**
     * @brief Concatenates the nodes left to right.
     * @param root The root of the tree.
     * @return Void.
     */
    void concatenateTreeLtR(Node* const &root);

    /**
     * @brief Concatenates the nodes right to left.
     * @param root The root of the tree.
     * @return Void.
     */
    void concatenateTreeRtL(Node *const &root);

    /**
     * @brief Concatenates the nodes alternating left to right and right to left at each level (balanced).
     * @param root The root of the tree.
     * @return Void.
     */
    void concatenateTreeBalanced(Node *const &root);

    /**
     * @brief Concatenates the nodes alternating left to right and right to left at each node (flipped).
     * @param root The root of the tree.
     * @return Void.
     */
    void concatenateTreeFlipped(Node *const &root);

     /**
      * @brief Concatenates one node to the last one chained to the tree.
      * @param cursor The last concatenated node of the tree.
      * @param node The next node to be concatenated.
      * @return Void.
      */
    void concatenateNode(Node *&cursor, Node *const &node);

    ConcatenationType concatenationType_; /** The breadth first type used to concatenate the tree. */
};


#endif //MANIFOLD_SPLITTING_CONCATENATOR_H
