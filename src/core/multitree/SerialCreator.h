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
};

#endif //MANIFOLD_SPLITTING_SERIALCREATOR_H
