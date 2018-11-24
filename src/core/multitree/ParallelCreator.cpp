//
// Created by matteo on 22/11/18.
//

#include "ParallelCreator.h"
#include <omp.h>

ParallelCreator::ParallelCreator() : maxLevelToParallelize_(0)
{
}

void ParallelCreator::buildTreeLtR(const Mesh &mesh, Node *const &root)
{
    std::vector<bool> isNodeInserted(mesh.num_faces(), false);
    std::vector<Node *> treeNodes(mesh.num_faces(), nullptr);
    std::vector<Node *> currLeaves;
    std::vector<std::vector<CGAL::SM_Face_index>> descIds(mesh.num_faces());
    isNodeInserted[root->id_] = true;
    treeNodes[root->id_] = root;

    buildTreeTop(mesh,root,currLeaves,treeNodes,isNodeInserted,descIds);

    #pragma omp parallel num_threads(currLeaves.size()) default(none) firstprivate(currLeaves) shared(isNodeInserted,treeNodes,mesh,descIds)
    {
        Node *cursor = currLeaves[omp_get_thread_num()];
        Node *currNode = currLeaves[omp_get_thread_num()];

        while (currNode)
        {
            CGAL::Face_around_face_iterator<Mesh> fafBegin,fafEnd;

            for (boost::tie(fafBegin,fafEnd)=CGAL::faces_around_face(mesh.halfedge(currNode->id_),mesh); fafBegin != fafEnd; ++fafBegin)
            {

                bool flag = false;
                #pragma omp critical
                {
                    if (!isNodeInserted[*fafBegin]) {
                        isNodeInserted[*fafBegin] = true;
                        flag = true;
                    }
                }

                if(flag)
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
                    if (currNode->parent_->id_ != *fafBegin)
                    {
                        descIds[*fafBegin].emplace_back(currNode->id_);
                    }
                }

            }

            currNode = currNode->next_;
        }
    }

    insertNodeDescFromIds(treeNodes, descIds);
}

void ParallelCreator::buildTreeTop(const Mesh &mesh, Node *const &root,
                                   std::vector<Node *> &leaves,
                                   std::vector<Node *> &treeNodes,
                                   std::vector<bool> &isNodeInserted,
                                   std::vector<std::vector<CGAL::SM_Face_index>> &descIds)
{
    Node *currNode = root;
    Node *cursor = root;
    int currLevel = -1;
    leaves.emplace_back(root); // extreme case: reduce the parallel creation to the serial creation

    while(currNode->level_ < maxLevelToParallelize_)
    {
        if(currNode->level_ != currLevel)
        {
            ++currLevel;
            leaves.clear();
        }

        CGAL::Face_around_face_iterator<Mesh> fafBegin, fafEnd;

        for (boost::tie(fafBegin, fafEnd) = CGAL::faces_around_face(mesh.halfedge(currNode->id_), mesh);
             fafBegin != fafEnd; ++fafBegin)
        {
            if (!treeNodes[*fafBegin])
            {
                currNode->children_.emplace_back(new Node(*fafBegin));
                Node *currChildren = currNode->children_.back();
                currChildren->parent_ = currNode;
                currChildren->level_ = currChildren->parent_->level_ + 1;

                treeNodes[*fafBegin] = currChildren;
                isNodeInserted[*fafBegin] = true;
                leaves.emplace_back(currNode->children_.back());

                currChildren->prev_ = cursor;
                cursor->next_ = currChildren;
                cursor = currChildren;
            }
            else
            {
                if (currNode->parent_->id_ != *fafBegin)
                {
                    descIds[*fafBegin].emplace_back(currNode->id_);
                }
            }
        }

        currNode = currNode->next_;
    }
}

void ParallelCreator::buildTreeRtL(const Mesh &mesh, Node *const &root) {

}

void ParallelCreator::buildTreeBalanced(const Mesh &mesh, Node *const &root) {

}

void ParallelCreator::buildTreeFlipped(const Mesh &mesh, Node *const &root) {

}

void ParallelCreator::insertNodeDescFromIds(std::vector<Node *> &nodes, std::vector<std::vector<CGAL::SM_Face_index>> &ids)
{
    for(auto &node : nodes)
    {
        for(auto &id : ids[node->id_])
        {
            node->descendants_.emplace_back(nodes[id]);
        }
    }
}


