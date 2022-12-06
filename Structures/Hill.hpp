#pragma once

#include "CSCIx229.h"
#include "Structure.hpp"

class Hill : public Structure
{
public:
    Hill();

    void Create();
    void Draw(bool hasControl = true);

    float baseRadius = 0.5f;
    float topRadius = 0.1f; // Should be equal to tunnel radius
    float height = 0.5f;
    int n = 8; // Sides of circles
    int panels = 5; // Vertical layers
private:
    // Use different primitives as convenient
    std::vector<int> triIndices; // Indices to draw using GL_TRIANGLES
    std::vector<int> quadIndices; // Indices to draw using GL_QUADS

    void CreateCLHelper(std::vector<Vertex> &CLtoUse);
    void CreateCenterline();

    void PreCreate();
    void CreateSides();
    void CreateTunnel();

    void DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals);
};