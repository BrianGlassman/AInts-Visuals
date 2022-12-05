#include "Objects.hpp"

#include "CSCIx229.h"

float MVscale = 10; // MagicaVoxel uses a 1:10 scale

OBJ Objects::Mine;

OBJ::OBJ() {}

OBJ::OBJ(int id, float scale)
{
    // FIXME undefined reference for SOME REASON // NORELEASE
    // id = LoadOBJ("Objects/", filename.c_str()); // NORELEASE
    this->id = id;
    this->scale = scale * MVscale;

    // For some reason not having a print statement here breaks everything
    printf("Created OBJ with id %d\n", id);
}

void OBJ::Draw()
{
    glPushMatrix();
    glScalef(scale, scale, scale);
    glCallList(id);
    glPopMatrix();
}