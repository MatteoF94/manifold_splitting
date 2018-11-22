//
// Created by matteo on 05/09/18.
//

//
// Created by matteo on 05/09/18.
//

#include <string>
#include <fstream>
#include <InputManager.h>
#include <stopwatch.h>
#include <MeshManager.h>

std::string selectMesh(int mesh_idx) {

    switch (mesh_idx) {
        case 0:
            return "bunny";
        case 1:
            return "casting";
        case 2:
            return "cow";
        case 3:
            return "crank";
        case 4:
            return "dragon";
        case 5:
            return "hand";
        case 6:
            return "horse";
        case 7:
            return "rabbit";
        case 8:
            return "ramesses";
        case 9:
            return "venus";
        default:
            break;
    }
}

int main (int argc, char* argv[]) {

    std::string selected_mesh = selectMesh(0);
    std::string input_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".off";

    InputManager input_manager;
    MeshManager mesh_manager;
    Stopwatch stopwatch;
    double elapsed_time, total_time = 0;

    std::cout << "Reading mesh from .off file..." << std::endl;
    stopwatch.start();
    Mesh mesh = input_manager.readMeshFromOff(input_filename);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Converting mesh to METIS .mesh format..." << std::endl;
    stopwatch.start();
    input_manager.meshToMetisFormat(mesh,"../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".mesh");
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partitioning the finite dual graph..." << std::endl;
    std::string execution_call = std::string("/home/matteo/Downloads/parmetis-4.0.3/build/Linux-x86_64/programs/mtest") +
                                 std::string(" ../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".mesh") +
                                 std::string(" 2");
    stopwatch.start();
    system(execution_call.c_str());
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".mesh.part"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    input_manager.writeMeshToOff(meshes,std::string("../../data/Watermarking/" + selected_mesh + "/ParMETIS/" + selected_mesh));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;

    return 0;
}
