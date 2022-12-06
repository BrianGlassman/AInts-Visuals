#pragma once

#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

#include "CSCIx229.h"
#include "Vector.hpp"
#include "Structure.hpp"

// Adapted from Ex. 28, loadobj.c, and loadtexbmp.c

void InitOBJs();

namespace Objects {
struct Face
{
    int vIdxs[3]; // Vertex indices
    int cIdxs[3]; // Color indices
    int nIdxs[3]; // Normal indices
};
}

class OBJ
{
public:
    OBJ(std::string filename, float scale = 1);

    int id;
    float scale; // The *inverse* scale (i.e. scale = 100 will shrink the model)
    std::vector<Vector3> OBJnormals;
    std::vector<Vector3> OBJcolors;
    std::vector<Vector3> OBJvertices;
    std::vector<Objects::Face> OBJfaces;

private:
    std::vector<Vector3> colors;
};

namespace Objects {
    extern std::shared_ptr<OBJ> MineTopCap;
    extern std::shared_ptr<OBJ> MineBottomCap;
    extern std::shared_ptr<OBJ> MineFrontCap;
    extern std::shared_ptr<OBJ> MineBackCap;
    extern std::shared_ptr<OBJ> MineCore;
    extern std::shared_ptr<OBJ> MineLeftCap;
    // No right cap, always open

    // OBJ Farm;
}