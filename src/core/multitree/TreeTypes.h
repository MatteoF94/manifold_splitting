//
// Created by matteo on 21/11/18.
//

#ifndef MANIFOLD_SPLITTING_TREETYPES_H
#define MANIFOLD_SPLITTING_TREETYPES_H

#include <vector>
#include <memory>
#include <CGAL/Surface_mesh.h>

enum class CreationMode
{
    Serial,
    Parallel
};

enum class ConcatenationType {
    LtR,
    RtL,
    Balanced,
    Flipped,
    DepthFirst
};

struct Node
{
    CGAL::SM_Face_index id_{};
    int level_;
    int numberOfElements_;
    double nVolume_;

    bool isAdopted_;

    std::vector<Node*> children_;
    Node* parent_;
    std::vector<Node*> descendants_;
    std::vector<Node*> relatives_;

    Node* next_;
    Node* prev_;

    explicit Node(const CGAL::SM_Face_index &id) : id_(id),
                                                   level_(0),
                                                   numberOfElements_(1),
                                                   nVolume_(0.0),
                                                   isAdopted_(false),
                                                   parent_(nullptr),
                                                   next_(nullptr),
                                                   prev_(nullptr)
    {
    }
};

#endif //MANIFOLD_SPLITTING_TREETYPES_H
