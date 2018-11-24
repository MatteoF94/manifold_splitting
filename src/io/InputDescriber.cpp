//
// Created by matteo on 23/11/18.
//

#include "InputDescriber.h"
#include <spdlog/spdlog.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <boost/property_map/property_map.hpp>
#include <CGAL/boost/graph/Face_filtered_graph.h>

void InputDescriber::describeMesh(Mesh &mesh, VerbosityLevel verbosityLevel)
{
    spdlog::info("InputDescriber::describeMesh ---- mesh has {} vertices", mesh.num_vertices());
    spdlog::info("InputDescriber::describeMesh ---- mesh has {} faces", mesh.num_faces());
    spdlog::info("InputDescriber::describeMesh ---- mesh has {} edges", mesh.num_edges());

    if(verbosityLevel != VerbosityLevel::Low)
    {
        if (CGAL::is_closed(mesh)) {
            spdlog::info("InputDescriber::describeMesh ---- mesh is a closed surface");

            if (CGAL::Polygon_mesh_processing::is_outward_oriented(mesh)) {
                spdlog::info(
                        "InputDescriber::describeMesh ---- mesh is outward oriented (counterclockwise sequence of face vertices)");
            } else {
                spdlog::info(
                        "InputDescriber::describeMesh ---- mesh is inward oriented (clockwise sequence of face vertices)");
            }
        } else spdlog::info("InputDescriber::describeMesh ---- mesh is an open surface");

        typename Mesh::Property_map<Mesh::Face_index, CGAL::Color> faceColors;
        bool hasFaceColors;
        boost::tie(faceColors, hasFaceColors) = mesh.property_map<Mesh::Face_index, CGAL::Color>("f:color");

        if (hasFaceColors)
        {
            spdlog::info("InputDescriber::describeMesh ---- mesh has colored facets");

            if(verbosityLevel == VerbosityLevel::Extreme)
            {
                for(const auto &fd : mesh.faces())
                {
                    CGAL::Color color = faceColors[fd];
                    spdlog::info("InputDescriber::describeMesh ---- face {} has RGB color {}:{}:{}",fd,color.red(),color.green(),color.blue());
                }
            }
        }
        else
        {
            spdlog::info("InputDescriber::describeMesh ---- mesh has uncolored facets");
        }

        Mesh::Property_map <Mesh::face_index, int64_t> fccmap;
        fccmap = mesh.add_property_map<Mesh::face_index, int64_t>("f:CC").first;
        int numConnected = CGAL::Polygon_mesh_processing::connected_components(mesh, fccmap);

        spdlog::info("InputDescriber::describeMesh ---- mesh has {} connected components (facets connectivity)",
                     numConnected);

        if(verbosityLevel != VerbosityLevel::Medium)
        {
            for(int i = 0; i < numConnected; ++i)
            {
                CGAL::Face_filtered_graph<Mesh> ffg(mesh,i,fccmap);
                spdlog::info("InputDescriber::describeMesh ---- component {} has {} faces",i,num_faces(ffg));
            }


            if(verbosityLevel == VerbosityLevel::Extreme)
            {
                for(const auto &f : mesh.faces())
                {
                    spdlog::info("InputDescriber::describeMesh ---- face {} is in component {}",f,fccmap[f]);
                }
            }
        }
    }
}
