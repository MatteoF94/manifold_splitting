//
// Created by matteo on 25/08/18.
//

#include <CGAL/Polygon_mesh_processing/connected_components.h>
#include <CGAL/Polygon_mesh_processing/stitch_borders.h>
#include <fstream>
#include "MeshManager.h"

typedef boost::adjacency_list<boost::setS,boost::vecS,boost::undirectedS> Graph;

void MeshManager::meshToGraph(Mesh mesh) {

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
    int num = boost::connected_components(finiteDual,fccmap);
    std::cout << "The graph has " << num << " connected components (face connectivity)" << std::endl;

    // ----- UNCOMMENT below to show for each face its belonging component -----
    /*BOOST_FOREACH(vertex_descriptor2 f, vertices(finiteDual)) {
        std::cout << f << " in connected component " << fccmap[f] << std::endl;
    }*/

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

    std::cout << "\tinserting vertices and edges..." << std::endl;
    // Then associate the ID of the descriptor with it and the edges between vertices
    // NOTE: working with the dual or the finite dual is indifferent

    for (boost::tie(vb,ve)=vertices(finiteDual); vb!=ve; ++vb) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        // Build edges, checking for duplicates
        for (boost::tie(ai,ai_end)=adjacent_vertices(*vb,finiteDual);ai != ai_end; ++ai) {
            add_edge(boost::vertex(*vb,g),boost::vertex(*ai,g),g);
        }
    }

    std::cout << "\t-- graph has " << boost::num_vertices(g) << " vertices" << std::endl;
    std::cout << "\t-- graph has " << boost::num_edges(g) << " edges" << std::endl;
    std::cout << "DONE -- GRAPH CREATED" << std::endl << std::endl;
    //return g;
}

std::vector<Mesh> MeshManager::breakMesh(Mesh mesh, std::string divisionFileName) {

    std::ifstream infile(divisionFileName);
    std::cout << "OPENING AT: " << divisionFileName.c_str() << std::endl;
    if (!infile) {
        std::cerr << "Unable to open file at " << divisionFileName.c_str() << std::endl;
        exit(0);
    }

    std::string line;
    std::vector<int> flags;

    while(std::getline(infile,line)) {
        std::istringstream iss(line);
        int flag;
        if(!(iss >> flag)) break;
        flags.push_back(flag);
    }

    int num_parts = 1 + *std::max_element(flags.begin(),flags.end());
    //Mesh splittedMeshes[num_parts];
    std::vector<Mesh> splittedMeshes(num_parts);
    std::map<Point,boost::graph_traits<Mesh>::vertex_descriptor> newVertices[num_parts];
    std::unordered_map<boost::graph_traits<Mesh>::vertex_descriptor,std::pair<int,int>> assigned_vertices;

    BOOST_FOREACH(boost::graph_traits<Mesh>::vertex_descriptor vd, vertices(mesh)) {
        std::pair<int,int> tmp = {-1,-1};
        assigned_vertices.insert({vd,tmp});
    }

    for (boost::graph_traits<Mesh>::face_iterator face_iterator = mesh.faces_begin(); face_iterator != mesh.faces_end(); ++face_iterator) {
        //std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor>::const_iterator it = myMap.find(*face_iterator);
        int currMeshIdx = flags.at(*face_iterator);
        std::vector<boost::graph_traits<Mesh>::vertex_descriptor> vrtcs;

        CGAL::Vertex_around_face_iterator<Mesh> vafb,vafe;
        for(boost::tie(vafb,vafe) = CGAL::vertices_around_face(mesh.halfedge(*face_iterator),mesh);vafb != vafe;++vafb) {

            Point p = mesh.point(*vafb);

            if(assigned_vertices[*vafb].first == -1 || (assigned_vertices[*vafb].second != currMeshIdx && assigned_vertices[*vafb].first == 1)) {
                vrtcs.push_back(splittedMeshes[currMeshIdx].add_vertex(p));
                newVertices[currMeshIdx].insert(std::make_pair(p,vrtcs.back()));
                assigned_vertices[*vafb].first = 1;

                if(assigned_vertices[*vafb].second == -1)
                    assigned_vertices[*vafb].second = currMeshIdx;

            } else
                vrtcs.push_back(newVertices[currMeshIdx][p]);
        }

        // Exploiting the concept of RANGE (from boost) we can use directly the container
        splittedMeshes[currMeshIdx].add_face(vrtcs);
    }

    return splittedMeshes;
}

void MeshManager::breakMeshAndStitch(Mesh mesh, std::string divisionFileName) {

    std::ifstream infile(divisionFileName);
    std::cout << "OPENING AT: " << divisionFileName.c_str() << std::endl;
    if (!infile) {
        std::cerr << "Unable to open file at " << divisionFileName.c_str() << std::endl;
        return;
    }

    std::string line;
    std::vector<int> flags;

    while(std::getline(infile,line)) {
        std::istringstream iss(line);
        int flag;
        if(!(iss >> flag)) break;
        flags.push_back(flag);
    }

    int num_parts = 1 + *std::max_element(flags.begin(),flags.end());
    CGAL::Surface_mesh<Point> splittedMeshes[num_parts];
    std::map<Point,boost::graph_traits<Mesh>::vertex_descriptor> newVertices[num_parts];
    std::unordered_map<boost::graph_traits<Mesh>::vertex_descriptor,std::pair<int,int>> assigned_vertices;

    BOOST_FOREACH(boost::graph_traits<Mesh>::vertex_descriptor vd, vertices(mesh)) {
                    std::pair<int,int> tmp = {-1,-1};
                    assigned_vertices.insert({vd,tmp});
                }

    for (boost::graph_traits<Mesh>::face_iterator face_iterator = mesh.faces_begin(); face_iterator != mesh.faces_end(); ++face_iterator) {
        //std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor,boost::graph_traits<Graph>::vertex_descriptor>::const_iterator it = myMap.find(*face_iterator);
        int currMeshIdx = flags.at(*face_iterator);
        std::vector<boost::graph_traits<Mesh>::vertex_descriptor> vrtcs;

        CGAL::Vertex_around_face_iterator<Mesh> vafb,vafe;
        for(boost::tie(vafb,vafe) = CGAL::vertices_around_face(mesh.halfedge(*face_iterator),mesh);vafb != vafe;++vafb) {

            Point p = mesh.point(*vafb);

            vrtcs.push_back(splittedMeshes[currMeshIdx].add_vertex(p));
            newVertices[currMeshIdx].insert(std::make_pair(p,vrtcs.back()));

        }

        // Exploiting the concept of RANGE (from boost) we can use directly the container
        splittedMeshes[currMeshIdx].add_face(vrtcs);
    }

    for (int i = 0; i < num_parts; ++i) {
        std::cout << "Mesh " << i << " num vertices before: " << splittedMeshes[i].num_vertices() << std::endl;
        CGAL::Polygon_mesh_processing::stitch_borders(splittedMeshes[i]);
        std::cout << "Mesh " << i << " num vertices after: " << splittedMeshes[i].num_vertices() << std::endl;
    }
}

std::map<boost::graph_traits<Mesh>::face_descriptor,double> MeshManager::computeFacesArea(Mesh mesh) {

    std::map<boost::graph_traits<Mesh>::face_descriptor,double> faces_area_map;

    for (boost::graph_traits<Mesh>::face_iterator face_iterator = mesh.faces_begin(); face_iterator != mesh.faces_end(); ++face_iterator) {
        boost::graph_traits<Mesh>::face_descriptor curr_face_id = *face_iterator;
        Point vertices[3];
        int curr_vertex = 0;

        CGAL::Vertex_around_face_iterator<Mesh> vafb,vafe;
        for(boost::tie(vafb,vafe) = CGAL::vertices_around_face(mesh.halfedge(curr_face_id),mesh);vafb != vafe;++vafb) {

            Point p = mesh.point(*vafb);
            vertices[curr_vertex] = p;
            ++curr_vertex;
        }

        faces_area_map.insert({curr_face_id,Kernel::Compute_area_3()(vertices[0],vertices[1],vertices[2])});
    }

    return faces_area_map;
}
