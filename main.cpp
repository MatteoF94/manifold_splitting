#include <iostream>
#include "include/InputManager.h"
#include "include/GraphParser.h"

int main() {
    InputManager inputManager;

    inputManager.readMeshFromOff("../data/bunny.off");
    //inputManager.readMeshFromOff("./cube_quad.off");
    Graph graph = inputManager.meshToGraphDual();

    //GraphParser graphParser;
    //graphParser.convertDotToMetis("./miao.dot");

    inputManager.breakMesh(2,"../data/tmppartition2");

    return 0;
}