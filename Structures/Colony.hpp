#pragma once

#include <vector>
#include <memory> // shared_ptr
#include <unordered_map>

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
private:
    std::unordered_map<Vector3Int, std::shared_ptr<Structure>> children;

    void AddStructure(std::shared_ptr<Structure> structPtr, Vector3 center);

    // FIXME part of temp motion model
    void CenterlineHelper(std::vector<Vertex>& dstCL, bool usePerturbed);
    std::vector<Vertex> baseCenterline;
    std::vector<Vertex> centerline;
    std::unordered_map<Vector3Int, int> offsets; // Offset value to convert from cell-local CL index to Colony CL index
};