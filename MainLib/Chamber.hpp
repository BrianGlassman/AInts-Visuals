#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Chamber : public Model
{
public:
    void Draw();

    float radius = 0.5f;
};