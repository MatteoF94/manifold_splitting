//
// Created by matteo on 23/11/18.
//

#ifndef MANIFOLD_SPLITTING_INPUTDESCRIBER_H
#define MANIFOLD_SPLITTING_INPUTDESCRIBER_H

#include <types.h>

class InputDescriber {
public:
    enum class VerbosityLevel
    {
        Low,
        Medium,
        High,
        Extreme
    };

    void describeMesh(Mesh &mesh, VerbosityLevel verbosityLevel);

};


#endif //MANIFOLD_SPLITTING_INPUTDESCRIBER_H
