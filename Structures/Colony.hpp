#pragma once

#include <vector>
#include <memory>

#include "CSCIx229.h"
#include "util.hpp" // Model class
#include "Structure.hpp"
#include "Vertex.hpp"

class Colony : public Model
{
public:
    Colony();
    virtual void Create();
    void ApplyNoise(float offset[]);
    void DrawCenterlines();
    void Draw();
    void AddTunnel(Vector3 center);
    // FIXME temporary convenience until auto-generating is implemented
    void AddTunnel(Vector3 center, unsigned char sides);
    void AddTunnel(float x, float y, float z);
    // FIXME temporary convenience until auto-generating is implemented
    void AddTunnel(float x, float y, float z, unsigned char sides);
    void AddChamber(Vector3 center);
    // FIXME temporary convenience until auto-generating is implemented
    void AddChamber(Vector3 center, unsigned char sides);
    void AddChamber(float x, float y, float z);
    // FIXME temporary convenience until auto-generating is implemented
    void AddChamber(float x, float y, float z, unsigned char sides);
    void AddHill(Vector3 center);
    void AddHill(float x, float y, float z);

    // FIXME part of temp motion model
    std::vector<Vertex>* getCL();

    // FIXME this should not be public, but was needed for setting SIdx of endpoints
    std::vector<std::shared_ptr<Structure>> children;
private:
    void AddStructure(std::shared_ptr<Structure> structPtr, Vector3 center);

    // FIXME part of temp motion model
    std::vector<Vertex> baseCenterline;
    std::vector<Vertex> centerline;
    std::vector<int> offsets; // Offset value to convert from cell-local CL index to Colony CL index. Order matches children
};