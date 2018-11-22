//
// Created by matteo on 10/08/18.
//

#ifndef MANIFOLD_SPLITTING_TYPES_H
#define MANIFOLD_SPLITTING_TYPES_H

#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/boost/graph/Dual.h>
#include <CGAL/Surface_mesh.h>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef CGAL::Dual<Mesh> Dual;
typedef boost::graph_traits<Dual>::edge_descriptor edge_descriptor;

template <typename G>
struct noborder{
    noborder() : g(NULL) {}

    explicit noborder(G& g) : g(&g) {}

    bool operator() (const edge_descriptor& e) const {
        return !is_border(e,*g);
    }

    G* g;
};

typedef boost::filtered_graph<Dual,noborder<Mesh>> FiniteDual;

typedef boost::adjacency_list<boost::setS,boost::vecS,boost::undirectedS> Graph;

enum class TreeConcatenationType {
    LtR,
    RtL,
    Balanced,
    Flipped,
    DepthFirst
};

struct MultiTreeNode {

    boost::graph_traits<FiniteDual>::vertex_descriptor id_;

    int value_{1};
    double area_{0.0};
    int level_{0};
    bool valid_{true};
    bool propagated_{false};
    bool is_border_{false};
    bool is_adopted_{false};

    MultiTreeNode* parent_ = nullptr;
    MultiTreeNode* left_ = nullptr;
    MultiTreeNode* mid_ = nullptr;
    MultiTreeNode* right_ = nullptr;

    MultiTreeNode* next_ = nullptr;
    MultiTreeNode* prev_ = nullptr;

    std::vector<MultiTreeNode*> descendants_;
    std::vector<MultiTreeNode*> relatives_;

    MultiTreeNode() {}
};

#endif //MANIFOLD_SPLITTING_TYPES_H
