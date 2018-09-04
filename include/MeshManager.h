//
// Created by matteo on 25/08/18.
//

#ifndef MANIFOLD_SPLITTING_MESHMANAGER_H
#define MANIFOLD_SPLITTING_MESHMANAGER_H

#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "types.h"

class MeshManager {
public:
    Graph meshToGraph(Mesh mesh);
    std::vector<Mesh> breakMesh(Mesh mesh, std::string divisionFileName);
    void breakMeshAndStitch(Mesh mesh, std::string divisionFileName);
    std::map<boost::graph_traits<Mesh>::face_descriptor,double> computeFacesArea(Mesh mesh);
};


#endif //MANIFOLD_SPLITTING_MESHMANAGER_H
