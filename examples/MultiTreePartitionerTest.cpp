//
// Created by matteo on 10/08/18.
//
#include <MultiTreePartitioner.h>
#include <cstdio>
#include <stopwatch.h>
#include <fstream>
#include <MultiTreeManager.h>
#include <MeshManager.h>
#include <MTVisualizer.h>
#include <MTSerialCreator.h>
#include <RegularGraphsGen.h>
#include <core/multitree/TreeTypes.h>
#include <core/multitree/CreatorManager.h>
#include <spdlog/spdlog.h>
#include <io/InputManager.h>
#include <io/InputDescriber.h>

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

    /*RegularGraphsGen graph_gen(2097152);
    Graph miao = graph_gen.createKRegGraph(16);
    graph_gen.writeGraphMetis(miao,"../../data/regular.graph");
    exit(0);*/

    spdlog::info("Welcome to spdlog version {}.{}.{}!",SPDLOG_VER_MAJOR,SPDLOG_VER_MINOR,SPDLOG_VER_PATCH);
    spdlog::set_level(spdlog::level::debug);
    std::string selected_mesh = selectMesh(0);
    std::string input_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + ".off";
    //std::string input_filename("/home/matteo/Desktop/meshes/Castle.off");

    //InputManager input_manager;
            InputManager inputManager;

    MeshManager mesh_manager;
    MultiTreeManager tree_manager;
    std::unique_ptr<CreatorManager> creatorManager = std::make_unique<CreatorManager>();
    Node *roota = new Node(Mesh::face_index(0));

    Stopwatch stopwatch;
    double elapsed_time, total_time = 0;

    spdlog::info("MAIN ---- reading mesh");
    stopwatch.start();
    //Mesh mesh = input_manager.readMeshFromOff(input_filename);
    Mesh mesh;
    inputManager.readMeshFromOff(input_filename,mesh);
    InputDescriber describer;
    describer.describeMesh(mesh,InputDescriber::VerbosityLevel::High);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    spdlog::info("MAIN ---- mesh read in {} seconds", elapsed_time);
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    creatorManager->setCreationMode(CreationMode::Parallel);
    creatorManager->createMultiTree(mesh,roota);

    int i = 0;
    while(roota) {
        ++i;
        roota = roota->next_;
    }
    std::cout << i << std::endl;
    exit(0);

    std::cout << "Converting mesh to multi level tree..." << std::endl;
    tree_manager.setCreationModes(MultiTreeManager::CreationType::SERIAL,true);
    tree_manager.configAdoption(2,10,true,true);
    tree_manager.configCreation(MultiTreeManager::ChainingType::LTR);
    tree_manager.configCreation(MultiTreeManager::ChainingType::LTR,MultiTreeManager::ChainingType::LTR);
    stopwatch.start();
    MultiTreeNode *root = tree_manager.meshToTree(mesh);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    MTVisualizer visualizer;
    //visualizer.visualizeMesh(root,mesh);
    //exit(0);

    /*MultiTreeNode *lasti = root;
    int miao = 0;
    while (lasti != nullptr) {
        lasti = lasti->next;
        ++miao;
    }
    std::cout << miao << std::endl;
    if(miao != 5804)
        exit(1);*/
//}

    //std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids = mesh_manager.computeFacesCentroid(mesh);
    //tree_manager.visualizeMultiTree(root,centroids);

//exit(0);

    MultiTreeNode* last = root;
    int num_nodes = 5;//input_manager.getNumFaces();

    //exit(0);
            int col = 0;
    while (last->next_ != nullptr) {
        last = last->next_;
        col++;
    }
    std::cout << col << std::endl;

    int K = 8;
    int thresh = num_nodes / K;
    MultiTreePartitioner partitioner;
    partitioner.configParameters(10,thresh,40,K);

    std::cout << "Partitioning tree..." << std::endl;
    stopwatch.start();
    std::vector<int> group_ids = partitioner.partitionByNumber(last, root, num_nodes);
    //MTSerialCreator serialCreator(&tree_manager);
    //Dual dual(mesh);
    //FiniteDual finiteDual(dual, noborder<Mesh>(mesh));
    //std::vector<int> group_ids = serialCreator.createSerialTreeDF(finiteDual, mesh);
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;
    std::vector<std::vector<MultiTreeNode*>*>* roots_set = partitioner.getRoots();

    std::vector<MultiTreeNode*> roots(roots_set->size());
    for(auto &curr_root : *roots_set) {
        tree_manager.regenerateTree(curr_root,group_ids);
        MultiTreeNode* rootina = curr_root->operator[](0);
        tree_manager.addBordersToTree(rootina);
        roots.emplace_back(rootina);
    }

    std::cout << "Partitioning results: " << std::endl;
    std::vector<int> num_elem(K,0);

    for (int group_id : group_ids) {
        if(group_id != -1)
            num_elem.at(group_id)++;
    }

    for (int j = 0; j < K; j++)
        std::cout << "C" << j << ": " << num_elem.at(j) << std::endl;

    visualizer.visualizeSubMeshes(roots,mesh);
    exit(0);

    std::string partition_filename = "../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + "_mtp.txt";
    //std::string partition_filename = "../../data/castle_mtp.txt";

    std::ofstream outfile(partition_filename);
    int min_pos = static_cast<int>(std::distance(num_elem.begin(), std::min_element(num_elem.begin(), num_elem.end())));
    for (int group_id : group_ids) {
        if(group_id == -1)
            outfile << min_pos << std::endl;
        else
            outfile << group_id << std::endl;
    }

 //exit(0);
    std::cout << "Breaking the mesh..." << std::endl;
    stopwatch.start();
    std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/Watermarking/" + selected_mesh + "/" + selected_mesh + "_mtp.txt"));
    //std::vector<Mesh> meshes = mesh_manager.breakMesh(mesh,std::string("../../data/castle_mtp.txt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Writing the sub-meshes to .off files..." << std::endl;
    stopwatch.start();
    //input_manager.writeMeshToOff(meshes,std::string("../../data/Watermarking/" + selected_mesh + "/MTP/" + selected_mesh));
    //input_manager.writeMeshToOff(meshes,std::string("../../data/Castle/castle_mpt"));
    elapsed_time = stopwatch.stop();
    total_time = total_time + elapsed_time;
    std::cout << "DONE in " << elapsed_time << " seconds" << std::endl << std::endl;

    std::cout << "Partition done in " << total_time << " seconds" << std::endl;
}

/*#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <zconf.h>
#include <iostream>

#include <InputManager.h>

struct MyPosData {
    GLdouble x = 0;
    GLdouble y = 0;
};

void controls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(action == GLFW_REPEAT) {
        MyPosData *dataPtr = (MyPosData*)glfwGetWindowUserPointer(window);
        switch (key) {
            case GLFW_KEY_RIGHT:
                dataPtr->x -= 1;
                break;
            case GLFW_KEY_LEFT:
                dataPtr->x += 1;
                break;
            case GLFW_KEY_DOWN:
                dataPtr->y -= 1;
                break;
            case GLFW_KEY_UP:
                dataPtr->y += 1;
                break;
        }
    }
}

GLFWwindow* initWindow(const int resX, const int resY)
{
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "TEST", NULL, NULL);

    if(window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); // Depth Testing
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

void drawCube(GLFWwindow* window)
{
    GLfloat vertices[] =
            {
                    -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
                    1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
                    -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
                    -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
                    -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
                    -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
            };

    GLfloat colors[] =
            {
                    0, 0, 0,   0, 0, 1,   0, 1, 1,   0, 1, 0,
                    1, 0, 0,   1, 0, 1,   1, 1, 1,   1, 1, 0,
                    0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,
                    0, 1, 0,   0, 1, 1,   1, 1, 1,   1, 1, 0,
                    0, 0, 0,   0, 1, 0,   1, 1, 0,   1, 0, 0,
                    0, 0, 1,   0, 1, 1,   1, 1, 1,   1, 0, 1
            };

    MyPosData *dataPtr = (MyPosData*)glfwGetWindowUserPointer(window);
    static float alpha = 0;
    //attempt to rotate cube
    glRotatef(dataPtr->x, 0, 1, 0);
    glRotatef(dataPtr->y,1,0,0);

    // We have a color array and a vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    // Send data : 24 vertices
    glDrawArrays(GL_QUADS, 0, 24);

    // Cleanup states
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    alpha += 1;
}

void display( GLFWwindow* window )
{
    while(!glfwWindowShouldClose(window))
    {
        // Scale to window size
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // Draw stuff
        glClearColor(0.0, 0.8, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective( 60, (double)windowWidth / (double)windowHeight, 0.1, 100 );

        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0,0,-5);

        drawCube(window);

        // Update Screen
        glfwSwapBuffers(window);

        // Check for any input, or window movement
        glfwPollEvents();
    }
}*/

/*int main(int argc, char** argv)
{
    //GLFWwindow* window = initWindow(1024, 620);
    //MyPosData my_pos_data;
    //glfwSetWindowUserPointer(window,&my_pos_data);
    //if( NULL != window )
    {
        InputManager input_manager;
        std::string input_filename = "../../data/Watermarking/hand/hand.off";

        std::cout << "Reading mesh from .off file..." << std::endl;
        Mesh mesh = input_manager.readMeshFromOff(input_filename);
        //display( window );
    }
    //glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}*/