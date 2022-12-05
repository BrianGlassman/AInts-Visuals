#pragma once

#include <stdio.h>
#include <string>

#include "CSCIx229.h"

// Adapted from Ex. 28

float MVscale = 10; // MagicaVoxel uses a 1:10 scale

class OBJ
{
public:
    OBJ(std::string filename, float scale = 1)
    {
        id = LoadOBJ("Objects/", filename.c_str());
        this->scale = scale * MVscale;

        // For some reason not having a print statement here breaks everything
        printf("Created OBJ with id %d\n", id);
    }

    int id;
    float scale;

    void Draw()
    {
        glPushMatrix();
        glScalef(scale, scale, scale);
        glCallList(id);
        glPopMatrix();
    }
};

namespace Objects {
    extern OBJ Mine;
    // OBJ Farm;
}