#include <iostream>
#include "include/InputManager.h"
#include "include/GraphParser.h"

int main() {
    InputManager inputManager;

    inputManager.readMeshFromOff("./cube_quad.off");
    Graph graph = inputManager.meshToGraphDual();
    //std::cout << "my CGAL library is " << CGAL_VERSION_NR << " (1MMmmb1000)" << std::endl;

    GraphParser graphParser;
    graphParser.convertDotToMetis("./miao.dot");

    inputManager.breakMesh(graph);

    return 0;
}
/*#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polygon_mesh_processing/stitch_borders.h>
#include <iostream>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Kernel::Point_3 Point;

int main(int argc, char* argv[])
{
    const char* filename = (argc > 1) ? argv[1] : "../data/full_border_quads.off";
    std::ifstream input("../data/full_border_quads.off");


    Polyhedron myPoly;
    input >> myPoly;

    //myPoly.make_triangle(Point(1,2,1),Point(2,2,2),Point(0,0,0));
    //myPoly.make_triangle(Point(1,2,1),Point(2,2,2),Point(2,0,0));
    //myPoly.make_triangle(Point(1,2,1),Point(2,2,2),Point(0,0,0));
    std::cout << "Before stitching : " << std::endl;
    std::cout << "\t Number of vertices  :\t" << myPoly.size_of_vertices() << std::endl;
    //std::cout << "\t Number of halfedges :\t" << mesh.num_halfedges() << std::endl;
    //std::cout << "\t Number of facets    :\t" << mesh.num_faces() << std::endl;
    CGAL::Polygon_mesh_processing::stitch_borders(myPoly);
    std::cout << "Stitching done : " << std::endl;
    std::cout << "\t Number of vertices  :\t" << myPoly.size_of_vertices() << std::endl;
    //std::cout << "\t Number of halfedges :\t" << mesh.num_halfedges() << std::endl;
    //std::cout << "\t Number of facets    :\t" << mesh.num_faces() << std::endl;
    //std::ofstream output("mesh_stitched.off");
    //output << std::setprecision(17) << mesh;
    return 0;
}*/