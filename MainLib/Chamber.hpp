#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Chamber : Model
{
public:
    void Draw();

    float radius = 0.5f;
};