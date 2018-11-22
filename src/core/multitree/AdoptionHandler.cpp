/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file AdoptionHandler.cpp
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#include <stack>
#include <queue>
#include "AdoptionHandler.h"
#include <spdlog/spdlog.h>

/**
 * @brief Class constructor.
 */
AdoptionHandler::AdoptionHandler() : deepAdoptionEnabled_(false),
                                     multiAdoptionEnabled_(false),
                                     artifactDepth_(5),
                                     artifactWidth_(2)
{
    spdlog::debug("AdoptionHandler::AdoptionHandler ---- constructed");
}

/**
 * @brief Class destructor.
 */
AdoptionHandler::~AdoptionHandler()
{
    spdlog::debug("AdoptionHandler::~AdoptionHandler ---- destroyed");
}

/**
 * Sets the parameters for the adoption algorithm.
 */
void AdoptionHandler::configHandler(bool enableDeepAdoption, bool enableMultiAdoption, int artifactDepth, int artifactWidth)
{
    deepAdoptionEnabled_ = enableDeepAdoption;
    multiAdoptionEnabled_ = enableMultiAdoption;
    artifactDepth_ = artifactDepth;
    artifactWidth_ = artifactWidth;
}

/**
 * Moves the branches in the tree, such that only nodes with one child and generating an artifact (branch where, for
 * a given depth, at each level the children span is always below a threshold) can request adoption. A branch is
 * adopted only if its root is an only child.
 *
 * Different parameters are considered such as the artifact thickness and depth, the possibility to adopt multiple
 * times and the possibility to make an adopted node adopt another node (deep adoption).
 */
void AdoptionHandler::adoptBranches(Node *const &root)
{
    std::stack<Node*> nodeStack;
    Node *root_copy = root;
    nodeStack.emplace(root_copy);

    while(!nodeStack.empty())
    {
        Node* currNode = nodeStack.top();
        nodeStack.pop();
        std::vector<Node*> currChildren;
        int childrenCount = 0;

        for(auto childrenIter = currNode->children_.rbegin(); childrenIter != currNode->children_.rend(); ++childrenIter)
        {
            currChildren.emplace_back(*childrenIter);
            ++childrenCount;
        }

        if(childrenCount == 1 && currNode->descendants_.size() == 1 && currNode->descendants_[0]->parent_->children_.size() == 1)
        {
            Node* descendant = currNode->descendants_[0];
            if(isArtifact(currNode) && currNode->level_ != descendant->level_)
            {
                descendant->parent_->children_.clear();

                if(currNode->level_ == descendant->parent_->level_)
                {
                    descendant->parent_->descendants_.emplace_back(descendant);
                    descendant->relatives_.emplace_back(descendant->parent_);
                }
                else
                {
                    descendant->parent_->relatives_.emplace_back(descendant);
                    descendant->descendants_.emplace_back(descendant->parent_);
                }

                descendant->parent_ = currNode;

                currNode->children_.emplace_back(descendant);
                if(deepAdoptionEnabled_)
                {
                    currChildren.emplace_back(descendant);
                }

                currNode->descendants_.clear();
                descendant->relatives_.erase(
                        std::remove(descendant->relatives_.begin(), descendant->relatives_.end(), currNode),
                        descendant->relatives_.end());

                adjustDescAndRels(descendant);
            }

            if(deepAdoptionEnabled_)
            {
                for(auto &child : currChildren) nodeStack.push(child);
            }

        }
        else
        {
            for(auto &elem : currChildren) nodeStack.push(elem);
        }
    }
}

/**
 * Checks if a branch portion is an artifact or not. An artifact is characterized by a given length (measured root to
 * deepest leaf), and a width, that is the maximum number of nodes at a given level.
 */
bool AdoptionHandler::isArtifact(const Node *const &node)
{
    if(!multiAdoptionEnabled_ )//&& node->is_adopted_)
        return false;

    std::queue<const Node*> nodeQueue;
    const Node *nodeCopy = node;
    nodeQueue.emplace(nodeCopy);
    int currLevel = node->level_;
    int count = 0;

    while(!nodeQueue.empty())
    {
        const Node *currNode = nodeQueue.front();

        if(currNode->level_ - node->level_ >= artifactDepth_) return true;

        if(currNode->level_ != currLevel)
        {
            count = 0;
            currLevel = currNode->level_;
        }

        for(auto &child : currNode->children_)
        {
            nodeQueue.emplace(child);
            ++count;
        }

        if(count > artifactWidth_) return false;

        nodeQueue.pop();
    }

    return false;
}

/**
 * Inverts descendants and relatives of nodes that, after being adopted, are placed in upper levels than their original
 * parent.
 */
void AdoptionHandler::adjustDescAndRels(Node *const &node)
{
    std::queue<Node*> nodeQueue;
    Node *nodeCopy(node);
    nodeQueue.emplace(nodeCopy);

    while(!nodeQueue.empty())
    {
        Node* currNode = nodeQueue.front();
        currNode->level_ = currNode->parent_->level_ + 1;
        currNode->isAdopted_ = true;

        std::vector<int> idxsToDelete;
        for (int i = 0; i < currNode->relatives_.size(); ++i)
        {
            Node* relative = currNode->relatives_[i];

            //TODO avoid the ambiguous forward descendant situation that can happen for the "=" case
            if(relative->level_ >= currNode->level_)
            {
                relative->descendants_.erase(std::remove(relative->descendants_.begin(),relative->descendants_.end(),currNode),relative->descendants_.end());
                relative->relatives_.emplace_back(currNode);
                currNode->descendants_.emplace_back(relative);
                idxsToDelete.emplace_back(i);
            }
        }

        for (auto idx : idxsToDelete)
        {
            currNode->relatives_[idx] = currNode->relatives_.back();
            currNode->relatives_.pop_back();
        }

        for(auto &child : currNode->children_)
        {
            nodeQueue.emplace(child);
        }

        nodeQueue.pop();
    }
}