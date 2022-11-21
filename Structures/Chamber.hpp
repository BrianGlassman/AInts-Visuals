#pragma once

#include <unordered_set>

#include "CSCIx229.h"
#include "Structure.hpp"

class Chamber : public Structure
{
public:
    Chamber() : Chamber(63) {}
    // FIXME temporary convenience until auto-generating is implemented
    Chamber(unsigned char sides);

    void Create();
    void ApplyNoise(Noise* noise, float offset[]);
    void Draw();

    float radius = 0.5f;
    // Number of panels per side (each side is N x N panels). Must be at least 3 (2 edges + >=1 face)
    int panels = 5;
private:
    float panelWidth;
    std::unordered_set<int> armIndices;

    void FaceHelper(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm);
    void CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm);
    void CreateArm(int i0, bool f0, int i1, bool f1, int i2, bool f2);
};