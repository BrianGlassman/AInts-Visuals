#pragma once

#include <vector>
#include <memory> // shared_ptr
#include <unordered_map>

#include "CSCIx229.h"
#include "util.hpp" // Model class
#include "globals.hpp"
#include "Structure.hpp"
#include "Vertex.hpp"

class Colony : public Model
{
public:
    Colony();
    virtual void PreCreate();
    virtual void Create();
    void ApplyNoise();
    void DrawCenterlines();
    void Draw(bool hasControl = true);

    void AddTunnel(Vector3Int center) { AddStructure(center, StructureType::Tunnel); }
    void AddTunnel(int x, int y, int z) { AddStructure({x, y, z}, StructureType::Tunnel); }

    void AddChamber(Vector3Int center) { AddStructure(center, StructureType::Chamber); }
    void AddChamber(int x, int y, int z) { AddStructure({x, y, z}, StructureType::Chamber); }

    void AddHill(Vector3Int center) { AddStructure(center, StructureType::Hill); }
    void AddHill(int x, int y, int z) { AddStructure({x, y, z}, StructureType::Hill); }

    void AddStructure(Vector3Int center, StructureType type);
    void AddStructure(int x, int y, int z, StructureType type);

    void DeleteStructure(Vector3Int center);
    void DeleteStructure(int x, int y, int z);

    std::shared_ptr<Structure> GetChild(Vector3Int coords); // For BuildIndicator to draw existing structures for deletion

    // Part of temp motion model // NORELEASE
    std::vector<Vertex>* getCL();
private:
    std::unordered_map<Vector3Int, std::shared_ptr<Structure>> children;

    // Part of temp motion model // NORELEASE
    void CenterlineHelper(std::vector<Vertex>& dstCL, bool usePerturbed);
    std::vector<Vertex> baseCenterline;
    std::vector<Vertex> centerline;
    std::unordered_map<Vector3Int, int> offsets; // Offset value to convert from cell-local CL index to Colony CL index
};