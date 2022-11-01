#pragma once

#include "CSCIx229.h"

#define ORIGIN 0.0, 0.0, 0.0
#define X_AXIS 1.0, 0.0, 0.0
#define Y_AXIS 0.0, 1.0, 0.0
#define Z_AXIS 0.0, 0.0, 1.0

void DrawLitQuad(const float A[], const float B[], const float C[], const float D[]);

// Abstract class
class Model
{
public:
    float center[3] = {0.0f, 0.0f, 0.0f};

    virtual void Draw() = 0; // = 0 makes it pure virtual
};

class Cylinder : Model
{
public:
    Cylinder();

    bool top = true;
    bool sides = true;
    bool bottom = true;
    float radius = 0.5f;
    float height = 1.0f;
    int n = 12;

    void Draw();
    void SetTopCenter(float x, float y, float z);
    void SetBottomCenter(float x, float y, float z);
};

class Cube : Model
{
public:
    Cube();

    bool   right = true; // +X
    bool    left = true; // -X
    bool     top = true; // +Y
    bool  bottom = true; // -Y
    bool forward = true; // +Z
    bool    back = true; // -Z

    // Internal (i.e. center to wall) distance
    float radius = 0.5f;

    void Draw();
private:
    unsigned char sides;
};