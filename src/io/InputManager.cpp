//
// Created by matteo on 23/11/18.
//

#include "InputManager.h"
#include <spdlog/spdlog.h>
#include <fstream>

void InputManager::readMeshFromOff(const std::string &offFilename, Mesh &mesh)
{
    spdlog::info("InputManager::readMeshFromOff ---- reading mesh at {}", offFilename);

    std::ifstream offFile(offFilename);
    if (!offFile) {
        spdlog::error("InputManager::readMeshFromOff ---- unable to open file at {}", offFilename);
    }

    if (!offFile) {
        spdlog::error("InputManager::readMeshFromOFF ---- wrong file format");
    }
    else
    {
        offFile >> mesh;
    }
    offFile.close();
}