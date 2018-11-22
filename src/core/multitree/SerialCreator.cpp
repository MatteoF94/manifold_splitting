/********************************************
 *
 * Copyright (C) 2018 Matteo Frosi
 *
 ********************************************/

/**
 * @file SerialCreator.cpp
 * @author Matteo Frosi
 * @date 19 nov 2018
 */

#include "SerialCreator.h"
#include <spdlog/spdlog.h>

/**
  * @brief Class constructor.
  */
SerialCreator::SerialCreator()
{
    spdlog::debug("SerialCreator::SerialCreator ---- constructed");
}

/**
  * @brief Class destructor.
  */
SerialCreator::~SerialCreator()
{
    spdlog::debug("SerialCreator::~SerialCreator ---- destroyed");
}

/**
  * Overrides base class function.
  */
void SerialCreator::buildTreeLtR(const Mesh &mesh, Node *const &root)
{
    std::vector<Node*> treeNodes(mesh.num_faces(),nullptr);
    treeNodes[root->id_] = root;

    Node* cursor = root;
    Node* currNode = root;

    while (currNode)
    {
        CGAL::Face_around_face_iterator<Mesh> fafBegin,fafEnd;

        for (boost::tie(fafBegin,fafEnd)=CGAL::faces_around_face(mesh.halfedge(currNode->id_),mesh); fafBegin != fafEnd; ++fafBegin)
        {
            if(!treeNodes[*fafBegin])
            {
                currNode->children_.emplace_back(new Node(*fafBegin));
                Node *currChildren = currNode->children_.back();
                currChildren->parent_ = currNode;
                currChildren->level_ = currChildren->parent_->level_ + 1;

                treeNodes[*fafBegin] = currChildren;

                currChildren->prev_ = cursor;
                cursor->next_ = currChildren;
                cursor = currChildren;

            }
            else
            {
                insertNodeDescAndRels(currNode,fafBegin,treeNodes);
            }

        }

        currNode = currNode->next_;
    }
}

/**
  * Overrides base class function.
  */
void SerialCreator::buildTreeRtL(const Mesh &mesh, Node *const &root)
{
    std::vector<Node*> treeNodes(mesh.num_faces(),nullptr);
    treeNodes[root->id_] = root;

    Node* cursor = root;
    Node* currNode = root;

    while (currNode)
    {
        CGAL::Face_around_face_iterator<Mesh> fafBegin,fafEnd;

        for (boost::tie(fafBegin,fafEnd)=CGAL::faces_around_face(mesh.halfedge(currNode->id_),mesh); fafBegin != fafEnd; ++fafBegin)
        {
            if(!treeNodes[*fafBegin])
            {
                currNode->children_.emplace_back(new Node(*fafBegin));
                Node *currChild = currNode->children_.back();
                currChild->parent_ = currNode;
                currChild->level_ = currChild->parent_->level_ + 1;

                treeNodes[*fafBegin] = currChild;
            }
            else
            {
                insertNodeDescAndRels(currNode,fafBegin,treeNodes);
            }
        }

        std::reverse(currNode->children_.begin(),currNode->children_.end());
        for(auto &currChild : currNode->children_)
        {
            currChild->prev_ = cursor;
            cursor->next_ = currChild;
            cursor = currChild;
        }
        currNode = currNode->next_;
    }
}

/**
  * Overrides base class function.
  */
void SerialCreator::buildTreeBalanced(const Mesh &mesh, Node *const &root)
{
    std::vector<Node*> treeNodes(mesh.num_faces(),nullptr);
    treeNodes[root->id_] = root;

    Node* cursor(root);
    Node* oldCursor(root);
    Node* currNode(root);

    bool toFlip = false;

    while (currNode)
    {
        CGAL::Face_around_face_iterator<Mesh> fafBegin,fafEnd;

        for (boost::tie(fafBegin,fafEnd)=CGAL::faces_around_face(mesh.halfedge(currNode->id_),mesh); fafBegin != fafEnd; ++fafBegin)
        {
            if(!treeNodes[*fafBegin])
            {
                currNode->children_.emplace_back(new Node(*fafBegin));
                Node *currChildren = currNode->children_.back();
                currChildren->parent_ = currNode;
                currChildren->level_ = currChildren->parent_->level_ + 1;

                treeNodes[*fafBegin] = currChildren;
            }
            else
            {
                insertNodeDescAndRels(currNode,fafBegin,treeNodes);
            }

        }

        if (toFlip) {
            std::reverse(currNode->children_.begin(), currNode->children_.end());
            for (auto &child : currNode->children_) {
                child->prev_ = cursor;
                child->prev_->next_ = child;
                cursor = child;
            }
        }

        int currLevel = currNode->level_;

        if (!currNode->prev_) {
            currNode = cursor;
            toFlip = true;
        } else {
            if (currNode->prev_->level_ != currLevel) {
                if(oldCursor == cursor)
                    currNode = nullptr;
                else {
                    currNode = cursor;
                    oldCursor = cursor;
                    toFlip = !toFlip;
                }

            } else
                currNode = currNode->prev_;
        }
    }
}

/**
  * Overrides base class function.
  */
void SerialCreator::buildTreeFlipped(const Mesh &mesh, Node *const &root)
{
    std::vector<Node*> treeNodes(mesh.num_faces(),nullptr);
    treeNodes[root->id_] = root;

    Node* cursor = root;
    Node* currNode = root;

    bool toFlip = false;

    while (currNode)
    {
        CGAL::Face_around_face_iterator<Mesh> fafBegin,fafEnd;

        for (boost::tie(fafBegin,fafEnd)=CGAL::faces_around_face(mesh.halfedge(currNode->id_),mesh); fafBegin != fafEnd; ++fafBegin)
        {
            if(!treeNodes[*fafBegin])
            {
                currNode->children_.emplace_back(new Node(*fafBegin));
                Node *currChild = currNode->children_.back();
                currChild->parent_ = currNode;
                currChild->level_ = currChild->parent_->level_ + 1;

                treeNodes[*fafBegin] = currChild;
            }
            else
            {
                insertNodeDescAndRels(currNode,fafBegin,treeNodes);
            }
        }

        if(toFlip)
        {
            std::reverse(currNode->children_.begin(), currNode->children_.end());
            for (auto &currChild : currNode->children_)
            {
                currChild->prev_ = cursor;
                cursor->next_ = currChild;
                cursor = currChild;
            }
        }

        toFlip = !toFlip;
        currNode = currNode->next_;
    }
}

/**
 * @brief Set one of the current node descendant or relative, depending on the position.
 */
void SerialCreator::insertNodeDescAndRels(Node *const &node, const CGAL::Face_around_face_iterator<Mesh> &neighbour, const std::vector<Node*> &treeNodes)
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