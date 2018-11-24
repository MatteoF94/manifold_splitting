/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file Creator.cpp
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#include "Creator.h"
#include <spdlog/spdlog.h>

/**
  * @brief Class constructor.
  */
Creator::Creator() : concatenationType_(ConcatenationType::LtR)
{
    spdlog::debug("Creator::Creator | constructed");
}

/**
 * @brief Class destructor.
 */
Creator::~Creator()
{
    spdlog::debug("Creator::~Creator | destroyed");
}

/**
  * @brief Set the tree concatenation type between nodes.
  */
void Creator::configCreator(ConcatenationType concatenationType)
{
    concatenationType_ = concatenationType;
}

/**
  * @brief Selects which algorithm variant is used to build the tree, depending on the way the nodes should be linked.
  */
void Creator::buildTree(const Mesh &mesh, Node *const &root)
{
    switch (concatenationType_)
    {
        case ConcatenationType::LtR :
            spdlog::info("Creator::buildTree ---- LTR breadth first nodes creation");
            buildTreeLtR(mesh,root);
            break;
        case ConcatenationType::RtL :
            spdlog::info("Creator::buildTree ---- RTL breadth first nodes creation");
            buildTreeRtL(mesh,root);
            break;
        case ConcatenationType::Balanced :
            spdlog::info("Creator::buildTree ---- BALANCED breadth first nodes creation");
            buildTreeBalanced(mesh,root);
            break;
        case ConcatenationType::Flipped :
            spdlog::info("Creator::buildTree ---- FLIPPED breadth first nodes creation");
            buildTreeFlipped(mesh,root);
            break;
        default:
            break;
    }
}

/**
 * @brief Set one of the current node descendant or relative, depending on the position.
 */
void Creator::insertNodeDescAndRels(Node *const &node, const CGAL::Face_around_face_iterator<Mesh> &neighbour, const std::vector<Node*> &treeNodes)
{
    if (node->parent_->id_ != *neighbour)
    {
        Node *relativeNode = treeNodes[*neighbour];

        if(node->level_ > relativeNode->level_)
        {
            relativeNode->descendants_.push_back(node);
            node->relatives_.emplace_back(relativeNode);
        }

        if(node->level_ == relativeNode->level_)
        {
            if (std::find(relativeNode->relatives_.begin(), relativeNode->relatives_.end(), node) ==
                relativeNode->relatives_.end())
            {
                node->descendants_.emplace_back(relativeNode);
                relativeNode->relatives_.emplace_back(node);
            }
        }
    }
}
