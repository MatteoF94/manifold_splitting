//
// Created by matteo on 06/11/18.
//

#include "MTVisualizer.h"

void MTVisualizer::controls_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    auto *dataPtr = (ControlsData*)glfwGetWindowUserPointer(window);

    if(action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_SPACE:
                dataPtr->to_display = false;
                break;
            case GLFW_KEY_RIGHT_BRACKET:
                dataPtr->scale += 0.5;
                break;
            case GLFW_KEY_P:
                dataPtr->scale -= 0.5;
                break;
            case GLFW_KEY_A:
                dataPtr->translation_x -= 0.05;
                break;
            case GLFW_KEY_D:
                dataPtr->translation_x += 0.05;
                break;
            case GLFW_KEY_W:
                dataPtr->translation_y += 0.05;
                break;
            case GLFW_KEY_S:
                dataPtr->translation_y -= 0.05;
                break;
            default:
                break;
        }
    }

    if(action == GLFW_REPEAT) {

        switch (key) {
            case GLFW_KEY_RIGHT:
                dataPtr->roll -= 1;
                break;
            case GLFW_KEY_LEFT:
                dataPtr->roll += 1;
                break;
            case GLFW_KEY_DOWN:
                dataPtr->pitch -= 1;
                break;
            case GLFW_KEY_UP:
                dataPtr->pitch += 1;
                break;
            default:
                break;
        }
    }
}

GLFWwindow* MTVisualizer::initWindow(const int resX, const int resY) {
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return nullptr;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "3D MultiTree", nullptr, nullptr);

    if(window == nullptr)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    auto func = [](GLFWwindow* w, int k, int s, int a, int m) {
        static_cast<MTVisualizer*>(glfwGetWindowUserPointer(w))->controls_callback(w,k,s,a,m);
    };

    glfwSetKeyCallback(window,func);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); // Depth Testing
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

void MTVisualizer::drawTree(GLFWwindow* window, std::vector<GLfloat> coords, std::vector<GLfloat> colors) {
    auto *controls_data = (ControlsData*) glfwGetWindowUserPointer(window);

    /*---- Rotations ----*/
    glRotatef(controls_data->roll, 0, 1, 0);
    glRotatef(controls_data->pitch, 1, 0, 0);

    /*---- Translations ----*/
    glTranslatef(controls_data->translation_x, controls_data->translation_y, 0);

    /*---- Scaling ----*/
    glScalef(controls_data->scale, controls_data->scale, controls_data->scale);

    /*---- Draw the tree ----*/
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, coords.data());
    glColorPointer(4, GL_FLOAT, 0, colors.data());
    glDrawArrays(GL_TRIANGLES, 0, coords.size() / 3);

    /*---- Highlight the starting faces/node ----*/
    /*glPointSize(10.0);
    glBegin(GL_POINTS);
    glVertex3f(coords.at(0), coords.at(1), coords.at(2));
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glEnd();*/

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void MTVisualizer::drawLoop(GLFWwindow* window,std::vector<GLfloat> coords,std::vector<GLfloat> colors) {

    ControlsData controls_data;
    glfwSetWindowUserPointer(window,&controls_data);
    GLint windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    int i = 0;
    while (controls_data.to_display) {

        // Draw stuff
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();

        gluPerspective(60, (double) windowWidth / (double) windowHeight, 0.1, 100);

        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0, 0, -5);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        drawTree(window,coords,colors);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

        // Update Screen
        glfwSwapBuffers(window);

        // Check for any input, or window movement
        glfwPollEvents();

        /*++i;
        if(i == 50)
            controls_data.to_display = false;*/
    }
}

void MTVisualizer::visualize3DTree(MultiTreeNode *root, std::map<boost::graph_traits<Mesh>::face_descriptor, Point> centroids){

    GLFWwindow* window = initWindow(1024, 620);
    if(!window) {
        std::cerr << "Window creation failed." << std::endl;
        exit(1);
    }

    std::vector<GLfloat> coords, colors;

    root = root->next_;

    while(root != nullptr) {
        Point p = centroids[root->id_];
        Point pp =  centroids[root->parent_->id_];
        std::vector<GLfloat> crds = {(GLfloat)p.x(),(GLfloat)p.y(),(GLfloat)p.z(),
                                     (GLfloat)pp.x(),(GLfloat)pp.y(),(GLfloat)pp.z()};
        coords.insert(coords.end(),crds.begin(),crds.end());

        std::vector<GLfloat> clrs = {0.f,0.f,0.f,1.f,0.f,0.f,0.f,1.f};
        colors.insert(colors.end(),clrs.begin(),clrs.end());

        root = root->next_;
    }

    //std::cout << coords.size() << std::endl;
    // std::cout << colors.size() << std::endl;
    drawLoop(window,coords,colors);
    glfwDestroyWindow(window);
}

void MTVisualizer::compare3DTrees(MultiTreeNode *root_a, MultiTreeNode *root_b, std::map<boost::graph_traits<Mesh>::face_descriptor, Point> centroids){

    GLFWwindow* window = initWindow(1024, 620);
    if(!window) {
        std::cerr << "Window creation failed." << std::endl;
        exit(1);
    }

    std::vector<GLfloat> coords, colors;

    root_a = root_a->next_;
    root_b = root_b->next_;

    while(root_a != nullptr) {
        Point p = centroids[root_a->id_];
        Point pp =  centroids[root_a->parent_->id_];
        std::vector<GLfloat> crds = {(GLfloat)p.x(),(GLfloat)p.y(),(GLfloat)p.z(),
                                     (GLfloat)pp.x(),(GLfloat)pp.y(),(GLfloat)pp.z()};
        coords.insert(coords.end(),crds.begin(),crds.end());

        std::vector<GLfloat> clrs = {1.f,0.f,0.f,0.5f,1.f,0.f,0.f,0.5f};
        colors.insert(colors.end(),clrs.begin(),clrs.end());

        root_a = root_a->next_;
    }

    while(root_b != nullptr) {
        Point p = centroids[root_b->id_];
        Point pp =  centroids[root_b->parent_->id_];
        std::vector<GLfloat> crds = {(GLfloat)p.x(),(GLfloat)p.y(),(GLfloat)p.z(),
                                     (GLfloat)pp.x(),(GLfloat)pp.y(),(GLfloat)pp.z()};
        coords.insert(coords.end(),crds.begin(),crds.end());

        std::vector<GLfloat> clrs = {0.f,1.f,0.f,0.5f,0.f,1.f,0.f,0.5f};
        colors.insert(colors.end(),clrs.begin(),clrs.end());

        root_b = root_b->next_;
    }

    drawLoop(window,coords,colors);
}

void MTVisualizer::visualizeMesh(MultiTreeNode *root, Mesh mesh) {

    GLFWwindow* window = initWindow(1024, 620);
    if(!window) {
        std::cerr << "Window creation failed." << std::endl;
        exit(1);
    }

    std::vector<GLfloat> coords, colors;

    while(root != nullptr) {
        boost::graph_traits<Mesh>::face_descriptor fdsc = root->id_;

        CGAL::Vertex_around_face_iterator<Mesh> vafb,vafe;
        for(boost::tie(vafb,vafe) = CGAL::vertices_around_face(mesh.halfedge(fdsc),mesh);vafb != vafe;++vafb) {
            Point p = mesh.point(*vafb);
            std::vector<GLfloat> crds = {(GLfloat) p.x(), (GLfloat) p.y(), (GLfloat) p.z()};
            coords.insert(coords.end(), crds.begin(), crds.end());
        }

        if(root->is_border_) {
            std::vector<GLfloat> clrs = {1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f};
            colors.insert(colors.end(), clrs.begin(), clrs.end());
        } else {
            std::vector<GLfloat> clrs = {0.5f, 0.5f, 0.5f, .5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, .5f};
            colors.insert(colors.end(), clrs.begin(), clrs.end());
        }
        root = root->next_;
    }

    drawLoop(window,coords,colors);
    glfwDestroyWindow(window);
}

void MTVisualizer::visualizeSubMeshes(std::vector<MultiTreeNode *> roots, Mesh mesh) {
    GLFWwindow* window = initWindow(1024, 620);
    if(!window) {
        std::cerr << "Window creation failed." << std::endl;
        exit(1);
    }

    std::vector<GLfloat> coords, colors;

    for(auto &root : roots) {
        while (root) {
            boost::graph_traits<Mesh>::face_descriptor fdsc = root->id_;

            CGAL::Vertex_around_face_iterator<Mesh> vafb, vafe;
            for (boost::tie(vafb, vafe) = CGAL::vertices_around_face(mesh.halfedge(fdsc), mesh); vafb != vafe; ++vafb) {
                Point p = mesh.point(*vafb);
                std::vector<GLfloat> crds = {(GLfloat) p.x(), (GLfloat) p.y(), (GLfloat) p.z()};
                coords.insert(coords.end(), crds.begin(), crds.end());
            }

            if (root->is_border_) {
                std::vector<GLfloat> clrs = {1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f};
                colors.insert(colors.end(), clrs.begin(), clrs.end());
            } else {
                std::vector<GLfloat> clrs = {0.7f, 0.7f, 0.7f, .5f, 0.7f, 0.7f, 0.7f, 1.0f, 0.7f, 0.7f, 0.7f, .5f};
                colors.insert(colors.end(), clrs.begin(), clrs.end());
            }
            root = root->next_;
        }
    }

    drawLoop(window,coords,colors);
    glfwDestroyWindow(window);
}