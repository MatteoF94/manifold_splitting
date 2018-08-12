//
// Created by matteo on 26/06/18.
//

#ifndef MANIFOLD_RECONSTRUCTION_INPUTMANAGER_H
#define MANIFOLD_RECONSTRUCTION_INPUTMANAGER_H


#include <string>
#include <unordered_map>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <types.h>

typedef boost::graph_traits<Mesh>::vertex_descriptor vertex_descriptor;

/*struct customVertex {
    boost::graph_traits<FiniteDual>::vertex_descriptor descriptor;
    int id;
};*/

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

    MultiTreeNode* meshToMultiTree();

private:
    Mesh inputMesh;
    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor> myMap;

};


#endif //MANIFOLD_RECONSTRUCTION_INPUTMANAGER_H
