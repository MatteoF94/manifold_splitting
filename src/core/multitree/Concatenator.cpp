/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file Concatenator.cpp
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#include <queue>
#include "Concatenator.h"
#include <spdlog/spdlog.h>

/**
  * @brief Class constructor.
*/
Concatenator::Concatenator() : concatenationType_(ConcatenationType::LtR)
{
    spdlog::debug("Concatenator::Concatenator ---- constructed");
}

/**
  * @brief Class destructor.
*/
Concatenator::~Concatenator()
{
    spdlog::debug("Concatenator::~Concatenator ---- destroyed");
}

/**
 * Set the concatenation type.
 */
void Concatenator::setConcatenationType(const ConcatenationType &concatenationType)
{
    concatenationType_ = concatenationType;
}

/**
 * Concatenates the nodes of a tree, setting prev and next for each.
 */
void Concatenator::concatenateTree(Node *const &root)
{
    switch (concatenationType_) {
        case ConcatenationType::LtR :
            spdlog::info("Concatenator::concatenateTree ---- linking nodes LTR breadth first");
            concatenateTreeLtR(root);
            break;
        case ConcatenationType::RtL :
            spdlog::info("Concatenator::concatenateTree ---- Linking nodes RTL breadth first");
            concatenateTreeRtL(root);
            break;
        case ConcatenationType::Balanced :
            spdlog::info("Concatenator::concatenateTree ---- Linking nodes BALANCED breadth first");
            concatenateTreeBalanced(root);
            break;
        case ConcatenationType::Flipped :
            spdlog::info("Concatenator::concatenateTree ---- Linking nodes FLIPPED breadth first");
            concatenateTreeFlipped(root);
            break;
        default:
            break;
    }

    spdlog::info("Concatenator::concatenateTree ---- finished linking nodes");
}

/**
 * Concatenates the nodes left to right.
 */
void Concatenator::concatenateTreeLtR(Node *const &root)
{
    Node *cursor(root);
    Node *currNode(root);

    while(currNode)
    {
        for(auto &child : currNode->children_)
        {
            concatenateNode(cursor,child);
        }

        currNode = currNode->next_;
    }
}

/**
 * Concatenates the nodes right to left.
 */
void Concatenator::concatenateTreeRtL(Node *const &root)
{
    Node *cursor(root);
    Node *currNode(root);

    while(currNode)
    {
        for(auto childRIter = currNode->children_.rbegin(); childRIter != currNode->children_.rend(); ++childRIter)
        {
            concatenateNode(cursor,*childRIter);
        }

        currNode = currNode->next_;
    }
}

/**
 * Concatenates the nodes alternating left to right and right to left at each level (balanced).
 */
void Concatenator::concatenateTreeBalanced(Node *const &root)
{
    bool toFlip = false;
    Node *currNode(root);
    Node *cursor(root);
    Node *oldCursor(root);

    for(auto &child : currNode->children_)
    {
        concatenateNode(cursor,child);
    }

    currNode = cursor;

    while(currNode)
    {
        std::vector<Node*> currChildren;

        for(auto &child : currNode->children_)
        {
            if(toFlip)
            {
                currChildren.emplace_back(child);
            }
            else
            {
                concatenateNode(cursor,child);
            }
        }

        if (toFlip)
        {
            std::reverse(currChildren.begin(), currChildren.end());
            for (auto &child : currChildren)
            {
                concatenateNode(cursor,child);
                child = nullptr;
                delete child;
            }
            currChildren.clear();
        }

        int currLevel = currNode->level_;

        if (currNode->prev_->level_ != currLevel)
        {
            if (oldCursor == cursor)
            {
                currNode = nullptr;
            }
            else
            {
                currNode = cursor;
                oldCursor = cursor;
                toFlip = !toFlip;
            }
        }
        else
        {
            currNode = currNode->prev_;
        }
    }
}

/**
 * Concatenates the nodes alternating left to right and right to left at each node (flipped).
 */
void Concatenator::concatenateTreeFlipped(Node *const &root)
{
    bool toFlip = false;
    Node *cursor(root);
    Node *currNode(root);

    while(currNode)
    {
        if(toFlip)
        {
            for(auto childRIter = currNode->children_.rbegin(); childRIter != currNode->children_.rend(); ++childRIter)
            {
                concatenateNode(cursor,*childRIter);
            }
        }
        else
        {
            for(auto &child : currNode->children_)
            {
                concatenateNode(cursor,child);
            }
        }

        currNode = currNode->next_;
        toFlip = !toFlip;
    }
}

/**
 * Concatenates one node to the last one chained to the tree.
 */
void Concatenator::concatenateNode(Node *&cursor, Node *const &node)
{
    cursor->next_ = node;
    node->prev_ = cursor;
    cursor = node;
    cursor->next_ = nullptr;
}