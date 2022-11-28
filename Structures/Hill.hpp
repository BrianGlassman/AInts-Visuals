#pragma once

#include "CSCIx229.h"
#include "Structure.hpp"

class Hill : public Structure
{
public:
    void Draw();

    float baseRadius = 0.5f;
    float topRadius = 0.2f;
    float height = 0.5f;
    int n = 12;
};