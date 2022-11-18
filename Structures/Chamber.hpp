#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Chamber : public Model
{
public:
    Chamber();

    void Create();
    void Draw();

    float radius = 0.5f;
    // Number of panels per side (panels = 2 --> 4 quads per side). Must be at least 3
    int panels = 4; // FIXME something about sending more than 256 vertices breaks and doesn't draw them
private:
    float panelWidth;

    void CreateCorner(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void CreateEdge(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2);
};