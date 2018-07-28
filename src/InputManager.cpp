//
// Created by matteo on 26/06/18.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "../include/InputManager.h"
#include <CGAL/Simple_cartesian.h>
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
#include <queue>

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

// Read a mesh from a .off file and convert it to graph
void InputManager::readMeshFromOff(const std::string filename) {

    std::cout << "InputManager::readMeshFromOff" << std::endl;

    std::ifstream infile(filename);
    std::cout << "OPENING AT: " << filename.c_str() << std::endl;
    if (!infile) {
        std::cerr << "Unable to open file at " << filename.c_str() << std::endl;
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
    std::cout << "Loaded MESH properties: " << std::endl;
    std::cout << "- " << mesh.num_vertices() << " vertices" << std::endl;
    std::cout << "- " << mesh.num_faces() << " faces" << std::endl;
    std::cout << "- " << mesh.num_edges() << " edges" << std::endl;


    typename Mesh::Property_map<Mesh::Face_index, CGAL::Color> fcolors;
    bool has_fcolors;
    boost::tie(fcolors, has_fcolors) = mesh.property_map<Mesh::Face_index, CGAL::Color >("f:color");
    std::cout << "- " << (has_fcolors ? "is colored" : "is not colored") << std::endl;


    BOOST_FOREACH(face_descriptor fd, faces(mesh)){
        if(has_fcolors)
        {
            CGAL::Color color = fcolors[fd];
            std::cout << "face: " << fd << " has RGB color: " << color << std::endl;
        }
    }

    // Find connected components of the mesh
    Mesh::Property_map<face_descriptor,int> fccmap;
    fccmap = mesh.add_property_map<face_descriptor,int>("f:CC").first;
    int num = CGAL::Polygon_mesh_processing::connected_components(mesh,fccmap);

    std::cout << "- " << num << " connected components (face connectivity): " << std::endl;
    for (int i = 0; i < num; i++) {
        std::cout << "  \tcomponent " << i << " has ";
        CGAL::Face_filtered_graph<Mesh> ffg(mesh,i,fccmap);
        std::cout << num_faces(ffg) << " faces" << std::endl;
    }


    // ----- UNCOMMENT below to show to which component each face belongs to -----
    /*std::cout << "Belonging component for each face:" << std::endl;
    BOOST_FOREACH(face_descriptor f, faces(mesh)) {
        std::cout << "\tface " << f << " is in connected component " << fccmap[f] << std::endl;
    }*/
}

Graph InputManager::meshToGraphDual() {
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

    // ----- UNCOMMENT below to show that the vertices of the finite dual have the same descriptors of the primal -----
    /*BOOST_FOREACH(boost::graph_traits<FiniteDual>::vertex_descriptor fdvd, vertices(finiteDual)) {
        std::cout << fdvdv << std::endl;
    }*/

    // ----- UNCOMMENT below to show edges of both primal and dual -----
    /*std::cout << "The edges in primal and dual with source and target" << std::endl;
    BOOST_FOREACH(edge_descriptor e, edges(finiteDual)) {
        std::cout << e << " in primal: " << source(e,mesh) << " -- " << target(e,mesh) << "    "
                  <<      " in dual  : " << source(e,finiteDual) << " -- " << target(e,finiteDual) << std::endl;
    }*/

    // ----- UNCOMMENT below to show edges of the dual mesh (excluding the dummy edges) -----
    /*std::cout << "Edges of the finite dual graph" << std::endl;
    BOOST_FOREACH(boost::graph_traits<FiniteDual>::edge_descriptor e, edges(finiteDual)) {
       std::cout << e << ": " << source(e,finiteDual) << " -- " << target(e,finiteDual) << std::endl;
    }*/

    typedef boost::graph_traits<FiniteDual>::vertex_descriptor vertex_descriptor_dual;
    Mesh::Property_map<vertex_descriptor_dual,int> fccmap;
    fccmap = mesh.add_property_map<vertex_descriptor_dual,int>("f:CC").first;
    int num = connected_components(finiteDual,fccmap);
    std::cout << "The graph has " << num << " connected components (face connectivity)" << std::endl;

    // ----- UNCOMMENT below to show for each face its belonging component -----
    /*BOOST_FOREACH(vertex_descriptor2 f, vertices(finiteDual)) {
        std::cout << f << " in connected component " << fccmap[f] << std::endl;
    }*/

    mesh.add_property_map<face_descriptor,CGAL::Color>("f:color<").first;

    // Create a map face-identifier for the loaded mesh
    boost::property_map<Mesh,boost::face_index_t >::type face_id_map = get(boost::face_index,mesh);
    std::map<boost::graph_traits<Mesh>::face_descriptor,int> face_map;

    for (boost::graph_traits<Mesh>::face_iterator face_it = mesh.faces_begin(); face_it != mesh.faces_end(); ++face_it) {
        int curr_idx = get(face_id_map,(*face_it));
        face_map.insert(std::make_pair((*face_it),curr_idx));
    }

    // ----- UNCOMMENT below to show the association between descriptors and face indices in the finite dual mesh -----
    /*std::map<boost::graph_traits<Mesh>::face_descriptor,int>::iterator face_map_it = face_map.begin();
    while(face_map_it != face_map.end()) {
        std::cout << "Descriptor: " << face_map_it->first << ", ID: " << face_map_it->second << std::endl;
        face_map_it++;
    }*/

    typedef boost::graph_traits<FiniteDual>::vertex_iterator vertex_iterator;
    vertex_iterator vb,ve;

    /*for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {
        std::cout << "Current vertex of the finite dual: " << (*vb) << std::endl;
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;
        std::cout << "\tAdjacent vertices: " << std::endl;
        for (boost::tie(ai,ai_end)=adjacent_vertices(*vb,finiteDual);ai != ai_end; ++ai) {
            std::cout << "\t\t" << (*ai) << std::endl;
        }

        boost::graph_traits<FiniteDual>::out_edge_iterator ei,ei_end;
        std::cout << "\tOutgoing edges: " << std::endl;
        for (boost::tie(ei,ei_end)=out_edges(*vb,finiteDual);ei != ei_end; ++ei) {
            std::cout << "\t\t" << (*ei) << std::endl;
        }
        std::cout << std::endl;
    }*/


    typedef boost::graph_traits<FiniteDual>::edge_iterator edge_iterator;
    edge_iterator eb,ee;

    // ----- UNCOMMENT below to show edges of the finite dual mesh -----
    /*for (boost::tie(eb,ee)=edges(finiteDual); eb!=ee; ++eb) {
        std::cout << " ---- " << (*eb) << std::endl;
    }*/

    // Graph creation
    Graph g;

    std::cout << "CREATING graph from the dual mesh:" << std::endl;
    std::cout << "\tinserting vertices..." << std::endl;

    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor,boost::graph_traits<FiniteDual>::vertex_descriptor> idDesc;
    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor> descId;
    // First add the custom vertices to the graph initializing them with the corresponding mesh descriptor
    int cont = 0;
    int miao[5] = {1,2,3,4,5};

    for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {
        boost::graph_traits<Graph>::vertex_descriptor u = boost::add_vertex(g);
        if(cont == 2)
            std::cout << miao[u] << std::endl;
        idDesc.insert({u,*vb});
        descId.insert({*vb,u});
        cont ++;
    }
    myMap = descId;
    std::cout << "\tDONE -- graph has " << g.vertex_set().size() << " vertices" << std::endl;

    std::cout << "\tinserting edges..." << std::endl;
    // Then associate the ID of the descriptor with it and the edges between vertices
    // NOTE: working with the dual or the finite dual is indifferent

    for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        // Build edges, checking for duplicates
        for (boost::tie(ai,ai_end)=adjacent_vertices(*vb,finiteDual);ai != ai_end; ++ai) {
            std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor >::const_iterator source = descId.find(*vb);
            std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor>::const_iterator target = descId.find(*ai);
                add_edge(source->second, target->second, g);
            //}
        }
    }
    std::cout << "\tDONE -- graph has " << boost::num_edges(g) << " edges" << std::endl;
    std::cout << "DONE -- GRAPH CREATED" << std::endl << std::endl;

    /*boost::write_graphviz(dot_file,g,[&] (auto& out, auto v) {
        out << "[id=\"" << g[v].id << "\"]";
    });*/
    return g;
}

void InputManager::breakMesh(int numParts, std::string divisionFileName, std::string output_filename) {
    Mesh splittedMeshes[numParts];
    std::map<Point,boost::graph_traits<Mesh>::vertex_descriptor> newVertices[numParts];
    std::vector<int> flags;

    std::ifstream infile(divisionFileName);
    std::cout << "OPENING AT: " << divisionFileName.c_str() << std::endl;
    if (!infile) {
        std::cerr << "Unable to open file at " << divisionFileName.c_str() << std::endl;
        return;
    }

    std::string line;

    while(std::getline(infile,line)) {
        std::istringstream iss(line);
        int flag;
        if(!(iss >> flag)) break;
        flags.push_back(flag);
    }

    int currIdx = 0;

    for (boost::graph_traits<Mesh>::face_iterator face_iterator = inputMesh.faces_begin(); face_iterator != inputMesh.faces_end(); ++face_iterator) {
        std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor>::const_iterator it = myMap.find(*face_iterator);
        int currMeshIdx = flags.at(it->second);
        currIdx++;

        boost::graph_traits<Mesh>::halfedge_descriptor hf = halfedge(*face_iterator,inputMesh);
        std::vector<boost::graph_traits<Mesh>::vertex_descriptor> vrtcs;
        for (Mesh::Halfedge_index hi : halfedges_around_face(hf,inputMesh)) {
            //std::cout << "Vertex index: " << target(hi,inputMesh) << std::endl;
            Point p = inputMesh.point(target(hi,inputMesh));
            if(newVertices[currMeshIdx].find(p)== newVertices[currMeshIdx].end()) {
                vrtcs.push_back(splittedMeshes[currMeshIdx].add_vertex(p));
                newVertices[currMeshIdx].insert(std::make_pair(p,vrtcs.back()));
            }
            else {
                vrtcs.push_back(newVertices[currMeshIdx][p]);
            }
        }
        // Exploiting the concept of RANGE (from boost) we can use directly the container
        splittedMeshes[currMeshIdx].add_face(vrtcs);
    }

    splittedMeshes[0].add_property_map<Mesh::Face_index, CGAL::Color >("f:color");
    typename Mesh::Property_map<Mesh::Face_index, CGAL::Color> fcolors;
    bool has_fcolors;
    boost::tie(fcolors, has_fcolors) = splittedMeshes[0].property_map<Mesh::Face_index, CGAL::Color >("f:color");

    BOOST_FOREACH(face_descriptor f, faces(splittedMeshes[0])){
            if(has_fcolors)
            {
                //std::cout << f << std::endl;
                //CGAL::Color color = fcolors[f];
                CGAL::Color col(100,100,100);
                put(fcolors,f,col);
                //std::cout << col << std::endl;
            }
        }

    for (int i = 0; i<numParts;i++) {
        std::cout << "Printing sub-graph " << i << ":" << std::endl;
        //boost::print_graph(splittedMeshes[i]);
        std::stringstream ss;
        ss << output_filename << "_" << i << ".off";
        std::ofstream outfile(ss.str());
        outfile << splittedMeshes[i];
    }
}
