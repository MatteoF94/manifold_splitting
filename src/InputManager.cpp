//
// Created by matteo on 26/06/18.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "../include/InputManager.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/boost/graph/helpers.h>
#include <CGAL/boost/graph/iterator.h>
#include <CGAL/boost/graph/Face_filtered_graph.h>

#include <CGAL/Polygon_mesh_processing/connected_components.h>

#include <boost/graph/connected_components.hpp>
#include <boost/foreach.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/bind.hpp>

typedef boost::graph_traits<Mesh>::face_descriptor face_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor edge_descriptor;

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

InputManager::InputManager() {
    //TODO
}

InputManager::~InputManager() {

}

// Search a vertex in a graph given a descriptor
VertexItr InputManager::findVertex(const Graph& g, const boost::graph_traits<FiniteDual>::vertex_descriptor& value) {
    VertexItr vi, vi_end;
    for (boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi) {
        if(g[*vi].descriptor == value) return vi;
    }
    return vi_end;
}

// Read a mesh from a .off file and convert it to graph
void InputManager::readMeshFromOff(const std::string filename) {

    std::cout << "InputManager::readMeshFromOff" << std::endl;

    std::ifstream infile(filename);
    std::cout << "OPENING AT: " << filename.c_str() << std::endl;
    if (!infile) {
        std::cerr << "Unable to open file at" << filename.c_str() << std::endl;
        return;
    }

    Mesh mesh;
    infile >> mesh;
    if (!infile) {
        std::cerr << "This is not a mesh!" << std::endl;
        return;
    }

    inputMesh = mesh;
    infile.close();
    std::cout << "Loaded MESH has: " << std::endl;
    std::cout << "-- " << mesh.num_vertices() << " vertices --" << std::endl;
    std::cout << "-- " << mesh.num_faces() << " faces --" << std::endl;
    std::cout << "-- " << mesh.num_edges() << " edges --" << std::endl;

    // Find connected components of the mesh
    Mesh::Property_map<face_descriptor,int> fccmap;
    fccmap = mesh.add_property_map<face_descriptor,int>("f:CC").first;
    int num = CGAL::Polygon_mesh_processing::connected_components(mesh,fccmap);

    std::cout << "The MESH has " << num << " connected components (face connectivity): " << std::endl;
    for (int i = 0; i < num; i++) {
        std::cout << "-- omponent " << i << " has ";
        CGAL::Face_filtered_graph<Mesh> ffg(mesh,i,fccmap);
        std::cout << num_faces(ffg) << " faces" << std::endl;
    }

    // ----- UNCOMMENT below to show to which component each face belongs to -----
    /*BOOST_FOREACH(face_descriptor f, faces(mesh)) {
        std::cout << f << " in connected component " << fccmap[f] << std::endl;
    }*/
}

void InputManager::meshToGraphDual() {
    Mesh mesh = inputMesh;
    Dual dual(mesh);
    FiniteDual finiteDual(dual,noborder<Mesh>(mesh));

    std::cout << "DUAL of the input MESH has: " << std::endl;
    std::cout << "-- " << num_vertices(dual) << " vertices --" << std::endl;
    std::cout << "-- " << num_faces(dual) << " faces --" << std::endl;
    std::cout << "-- " << num_edges(dual) << " edges --" << std::endl;

    std::cout << "The vertices of the dual are the faces in the primal graph" << std::endl;

    // ----- UNCOMMENT below to show faces descriptors, that are faces in the primal -----
    /*BOOST_FOREACH(boost::graph_traits<Dual>::vertex_descriptor dvd, vertices(dual)) {
        std::cout << dvd << std::endl;
    }*/

    // ----- UNCOMMENT below to show edges of both primal and dual -----
    /*std::cout << "The edges in primal and dual with source and target" << std::endl;
    BOOST_FOREACH(edge_descriptor e, edges(dual)) {
        std::cout << e << " in primal: " << source(e,mesh) << " -- " << target(e,mesh) << "    "
                  <<      " in dual  : " << source(e,finiteDual) << " -- " << target(e,finiteDual) << std::endl;
    }*/

    // ----- UNCOMMENT below to show edges of the dual mesh (excluding the dummy edges) -----
    /*std::cout << "Edges of the finite dual graph" << std::endl;
    BOOST_FOREACH(boost::graph_traits<FiniteDual>::edge_descriptor e, edges(finiteDual)) {
       std::cout << e << ": " << source(e,finiteDual) << " -- " << target(e,finiteDual) << std::endl;
    }*/

    typedef boost::graph_traits<FiniteDual>::vertex_descriptor vertex_descriptor2;
    Mesh::Property_map<vertex_descriptor2,int> fccmap;
    fccmap = mesh.add_property_map<vertex_descriptor2,int>("f:CC").first;
    int num = connected_components(finiteDual,fccmap);

    std::cout << "The graph has " << num << " connected components (face connectivity)" << std::endl;
    BOOST_FOREACH(vertex_descriptor2 f, vertices(finiteDual)) {
        std::cout << f << " in connected component " << fccmap[f] << std::endl;
    }

    BOOST_FOREACH(boost::graph_traits<FiniteDual>::vertex_descriptor v, vertices(finiteDual)) {
        std::cout << v << std::endl;
    }

    typedef boost::graph_traits<FiniteDual>::vertex_iterator vertex_iterator;
    vertex_iterator vb,ve;
    int miao = 0;

    // Create a map face-identifier for the loaded mesh
    boost::property_map<Mesh,boost::face_index_t >::type face_id_map = get(boost::face_index,mesh);
    std::map<boost::graph_traits<Mesh>::face_descriptor,int> face_map;

    for (boost::graph_traits<Mesh>::face_iterator face_it = mesh.faces_begin(); face_it != mesh.faces_end(); ++face_it) {
        int curr_idx = get(face_id_map,(*face_it));
        face_map.insert(std::make_pair((*face_it),curr_idx));
    }

    std::map<boost::graph_traits<Mesh>::face_descriptor,int>::iterator face_map_it = face_map.begin();
    while(face_map_it != face_map.end()) {
        std::cout << "Descriptor: " << face_map_it->first << ", ID: " << face_map_it->second << std::endl;
        face_map_it++;
    }

    for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {
        std::cout << (*vb) << std::endl;
        if (!miao) {
            boost::graph_traits<FiniteDual>::vertex_iterator it;
            it = boost::find(vertices(finiteDual),(*vb));
            std::cout << "I've found: " << (*it) << std::endl;
            //if (mapVert.find(*it) != mapVert.end())
            //    std::cout << "MIAOOOONE" << std::endl;
            miao++;
        }

        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;
        for (boost::tie(ai,ai_end)=adjacent_vertices(*vb,finiteDual);ai != ai_end; ++ai) {
            std::cout << "    " << (*ai) << std::endl;
        }

        boost::graph_traits<FiniteDual>::out_edge_iterator ei,ei_end;
        for (boost::tie(ei,ei_end)=out_edges(*vb,finiteDual);ei != ei_end; ++ei) {
            std::cout << "    " << (*ei) << std::endl;
        }
    }

    typedef boost::graph_traits<FiniteDual>::edge_iterator edge_iterator;
    edge_iterator eb,ee;

    for (boost::tie(eb,ee)=edges(finiteDual); eb!=ee; ++eb) {
        std::cout << " ---- " << (*eb) << std::endl;
    }

    //typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS,sampleVertex> Graph;//boost::graph_traits<FiniteDual>::vertex_descriptor> Graph;
    Graph g;//g(num_vertices(finiteDual));

    std::cout << "\nNum vertices: " << g.vertex_set().size() << std::endl;
    std::cout << "\nNum vertices graph: " << num_vertices(finiteDual) << std::endl;

    for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {
        boost::graph_traits<Graph>::vertex_descriptor u = boost::add_vertex(g);
        g[u].descriptor = (*vb);
    }

    for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {

        int curr_vertex_idx;
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        if(face_map.find(*vb) != face_map.end()) {
            curr_vertex_idx = face_map.at(*vb);
            g[*vb].id = curr_vertex_idx;
        } else {
            std::cerr << "Existing vertex not found" << std::endl;
            return;
        }
        for (boost::tie(ai,ai_end)=adjacent_vertices(*vb,finiteDual);ai != ai_end; ++ai) {
            if(face_map.find(*ai) != face_map.end())
                if(!boost::edge(*findVertex(g,*vb),*findVertex(g,*ai),g).second) {
                    VertexItr it = findVertex(g,*vb);
                    VertexItr it2 = findVertex(g,*ai);
                    add_edge(*it, *it2, g);
                }
        }
    }

    boost::print_graph(g);
    std::ofstream dot_file("miao.dot");
    boost::write_graphviz(dot_file,g,[&] (auto& out, auto v) {
        out << "[id=\"" << g[v].id << "\"]";
    });
}
