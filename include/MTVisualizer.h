//
// Created by matteo on 06/11/18.
//

#ifndef MANIFOLD_SPLITTING_MTVISUALIZER_H
#define MANIFOLD_SPLITTING_MTVISUALIZER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <types.h>

#include <vector>

class MTVisualizer {
public:
    MTVisualizer() {}
    void visualize3DTree(MultiTreeNode *root, std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids);
    void compare3DTrees(MultiTreeNode* root_a, MultiTreeNode* root_b, std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids);
    void visualizeMesh(MultiTreeNode *root, Mesh mesh);
    void visualizeSubMeshes(std::vector<MultiTreeNode*> roots, Mesh mesh);

private:
    struct ControlsData {
        GLfloat roll = 0;
        GLfloat pitch = 0;
        GLfloat scale = 1;
        GLfloat translation_x = 0;
        GLfloat translation_y = 0;
        bool to_display = true;
    };

    void controls_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    GLFWwindow* initWindow(const int resX, const int resY);
    void drawTree(GLFWwindow* window,std::vector<GLfloat> coords,std::vector<GLfloat> colors);
    void drawLoop(GLFWwindow* window,std::vector<GLfloat> coords,std::vector<GLfloat> colors);

    ControlsData controls_;
};


#endif //MANIFOLD_SPLITTING_MTVISUALIZER_H
