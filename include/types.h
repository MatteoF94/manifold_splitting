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

struct MultiTreeNode {
    int value;
    double area;
    int level;
    bool valid;
    bool propagated;
    boost::graph_traits<FiniteDual>::vertex_descriptor id;

    MultiTreeNode* left;
    MultiTreeNode* right;
    MultiTreeNode* parent;
    MultiTreeNode* next;
    MultiTreeNode* prev;

    MultiTreeNode* mid; // Only for the root node...

    std::vector<MultiTreeNode*> relatives;  // TODO use multi dim vector

    MultiTreeNode() : value(1), area(0.0), level(0), valid(true), propagated(false), left(nullptr), right(nullptr), mid(nullptr), parent(nullptr), next(nullptr), prev(nullptr) {

    }
};

#endif //MANIFOLD_SPLITTING_TYPES_H
