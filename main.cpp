#include <iostream>
#include "include/InputManager.h"

int main() {
    InputManager inputManager;

    inputManager.readMeshFromOff("./cube_quad.off");
    inputManager.meshToGraphDual();
    //std::cout << "my CGAL library is " << CGAL_VERSION_NR << " (1MMmmb1000)" << std::endl;
    return 0;
}