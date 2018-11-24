//
// Created by matteo on 24/11/18.
//

#include <queue>
#include "Utilities.h"

bool Utilities::checkTreeIntegrity(const Node *const &root, unsigned int numNodes) {

    if(!checkTreeConcatenation(root,numNodes)) return false;
    if(!checkTreeStructure(root,numNodes)) return false;
    if(!checkTreeSemantic(root)) return false;

    return true;
}

bool Utilities::checkTreeConcatenation(const Node *const &root, unsigned int numNodes)
{
    std::vector<bool> areNodesInserted(numNodes,false);
    const Node *currNode = root;

    while(currNode->next_)
    {
        if(areNodesInserted[currNode->id_])
            return false;
        else
            areNodesInserted[currNode->id_] = true;

        if(currNode->next_->prev_ != currNode) return false;

        currNode = currNode->next_;
    }
    areNodesInserted[currNode->id_] = true; // insert last node

    for(auto isIn : areNodesInserted)
        if(!isIn)
            return false;

    return true;
}

bool Utilities::checkTreeStructure(const Node *const &root, unsigned int numNodes)
{
    std::vector<bool> areNodesInserted(numNodes,false);
    std::queue<const Node *> nodeQueue;
    const Node *currNode;
    nodeQueue.emplace(root);

    if(root->descendants_.size() != 0 || root->relatives_.size() != 0) return false;

    while(!nodeQueue.empty())
    {
        currNode = nodeQueue.front();

        if(areNodesInserted[currNode->id_])
            return false;
        else
            areNodesInserted[currNode->id_] = true;

        for(auto &child : currNode->children_)
        {
            if(child->level_ != currNode->level_ + 1) return false;
            if(child->parent_ != currNode) return false;

            nodeQueue.emplace(child);
        }

        nodeQueue.pop();
    }

    for(auto isIn : areNodesInserted)
        if(!isIn)
            return false;

    return true;
}

bool Utilities::checkTreeSemantic(const Node *const &root)
{
    const Node *currNode = root->next_;

    if(root->children_.size() > 3) return false;

    while(currNode->next_)
    {
        unsigned int numChildren = currNode->children_.size();
        unsigned int numDescendants = currNode->descendants_.size();
        unsigned int numRelatives = currNode->relatives_.size();

        if(numChildren + numDescendants + numRelatives > 2) return false;

        currNode = currNode->next_;
    }

    return true;
}
