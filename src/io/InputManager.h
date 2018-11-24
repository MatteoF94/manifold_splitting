//
// Created by matteo on 23/11/18.
//

#ifndef MANIFOLD_SPLITTING_INPUTMANAGER_H
#define MANIFOLD_SPLITTING_INPUTMANAGER_H

#include <types.h>

class InputManager {
public:

    InputManager();
    ~InputManager();

    void readMeshFromOff(const std::string &offFilename, Mesh &mesh);

};


#endif //MANIFOLD_SPLITTING_INPUTMANAGER_H
