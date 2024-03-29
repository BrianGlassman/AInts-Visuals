#pragma once

#include <functional>
#include <unordered_set>

#include "CSCIx229.h"
#include "Structure.hpp"

class Chamber : public Structure
{
public:
    Chamber();

    virtual void PreCreate();
    virtual void Create(bool PrePost, bool createFaces, bool createArms);
    virtual void Create() override { Create(true, true, true); }
    virtual void ApplyNoise();
    virtual void Draw(bool hasControl = true);

    float radius = 0.5f;
    // Number of panels per side (each side is N x N panels). Must be at least 3 (2 edges + >=1 face)
    int panels = 5;
protected:
    float padding; // Padding between edge of chamber and edge of cell
    float padScale; // Scale factor to account for padding
    float panelWidth;
    std::unordered_set<int> armIndices;

    // FIXME should these really be properties? // NORELEASE
    // FIXME these should be linked, not hard-coded // NORELEASE
    int tunnelN = 8;
    int armPanels = 1; // Shouldn't be the same as the tunnel value, but should be related
    float tunnelRadius = 0.1f;
    // endFixme // NORELEASE

    // Use different primitives as convenient
    std::vector<int> triIndices; // Indices to draw using GL_TRIANGLES
    std::vector<int> quadIndices; // Indices to draw using GL_QUADS

    static Vector3 SetCoords(const float v0, const float v1, const float v2, int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void FacePointHelper(std::function<Vector3(const float, const float, const float)> BoundSetCoords, const float v0, const float v1, const float v2);
    void FaceHelper(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm, bool createArms);
    void CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm, bool createFaces, bool createArms);
    void CreateArm(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    void CreateCLHelper(std::vector<Vertex> &CLtoUse, int axis, bool flip);
    void CreateCenterline(int axis, bool flip);

    void DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals); // FIXME pass by reference // NORELEASE
};