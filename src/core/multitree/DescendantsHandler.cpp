//
// Created by matteo on 24/11/18.
//

#include "DescendantsHandler.h"

void DescendantsHandler::adjustDescendantsAndRelatives(Node *const &root)
{
    Node *currNode = root;

    while(currNode)
    {
        bool flag = true;
        std::vector<Node *> toRemove;

        for(auto &currDesc : currNode->descendants_)
        {
            if(currDesc->level_ == currNode->level_)
            {
                if (std::find(currDesc->descendants_.begin(), currDesc->descendants_.end(), currNode) ==
                    currDesc->descendants_.end())
                {
                    toRemove.emplace_back(currDesc);
                    flag = false;
                }
            }

            if(flag)
            {
                currDesc->relatives_.emplace_back(currNode);
            }
        }

        for(auto &descToRemove : toRemove)
        {
            currNode->descendants_.erase(std::remove(currNode->descendants_.begin(),currNode->descendants_.end(),descToRemove),currNode->descendants_.end());
        }

        currNode = currNode->next_;
    }
}
