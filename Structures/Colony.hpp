#pragma once

#include <vector>
#include <memory>

#include "CSCIx229.h"
#include "util.hpp" // Model class
#include "Structure.hpp"

class Colony : public Model
{
public:
    Colony();
    void ApplyNoise(float offset[]);
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

    Structure* getChild(int i) { return children[i].get(); }
private:
    std::vector<std::shared_ptr<Structure>> children;
    void AddStructure(std::shared_ptr<Structure> structPtr, Vector3 center);
};