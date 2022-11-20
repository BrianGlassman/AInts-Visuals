#pragma once

#include "CSCIx229.h"
#include "Structure.hpp"

class Chamber : public Structure
{
public:
    Chamber() : Chamber(63) {}
    // FIXME temporary convenience until auto-generating is implemented
    Chamber(unsigned char sides);

    void Create();
    void Draw();

    float radius = 0.5f;
    // Number of panels per side (each side is N x N panels). Must be at least 3 (2 edges + >=1 face)
    int panels = 5;
private:
    float panelWidth;

    void CreateCorner(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void CreateEdge(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void CreateArm(int i0, bool f0, int i1, bool f1, int i2, bool f2);
};