#pragma once

#include <stdio.h>
#include <string>

#include "CSCIx229.h"

// Adapted from Ex. 28

class OBJ
{
public:
    OBJ();
    OBJ(int id, float scale = 1);

    int id;
    float scale; // The *inverse* scale (i.e. scale = 100 will shrink the model)

    void Draw();
};

namespace Objects {
    extern OBJ Mine;
    // OBJ Farm;
}