//
// Created by matteo on 26/06/18.
//

#ifndef MANIFOLD_RECONSTRUCTION_INPUTMANAGER_H
#define MANIFOLD_RECONSTRUCTION_INPUTMANAGER_H


#include <string>
#include <unordered_map>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <CGAL/boost/graph/Dual.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef CGAL::Dual<Mesh> Dual;
typedef boost::graph_traits<Dual>::edge_descriptor edge_descriptor;
typedef boost::graph_traits<Mesh>::vertex_descriptor vertex_descriptor;

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

struct customVertex {
    boost::graph_traits<FiniteDual>::vertex_descriptor descriptor;
    int id;
};

typedef boost::adjacency_list<boost::setS,boost::vecS,boost::undirectedS> Graph;
typedef boost::graph_traits<Graph>::vertex_iterator VertexItr;

class InputManager {
public:

    InputManager();
    virtual ~InputManager();

    void readMeshFromOff(std::string filename);
    Graph meshToGraphPrimal(Mesh mesh);
    Graph meshToGraphDual();
    void breakMesh(int numParts, std::string divisionFileName, std::string output_filename);

private:
    Mesh inputMesh;
    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor> myMap;

};


#endif //MANIFOLD_RECONSTRUCTION_INPUTMANAGER_H
