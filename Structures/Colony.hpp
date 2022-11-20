#pragma once

#include <vector>
#include <memory>

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Colony : public Model
{
public:
    Colony();
    void ApplyNoise(Noise* noise, float offset[]);
    void Draw();
    void AddTunnel(float center[3]);
    // FIXME temporary convenience until auto-generating is implemented
    void AddTunnel(float center[3], unsigned char sides);
    void AddTunnel(float x, float y, float z);
    // FIXME temporary convenience until auto-generating is implemented
    void AddTunnel(float x, float y, float z, unsigned char sides);
    void AddChamber(float center[3]);
    // FIXME temporary convenience until auto-generating is implemented
    void AddChamber(float center[3], unsigned char sides);
    void AddChamber(float x, float y, float z);
    // FIXME temporary convenience until auto-generating is implemented
    void AddChamber(float x, float y, float z, unsigned char sides);
    void AddHill(float center[3]);
    void AddHill(float x, float y, float z);
private:
    std::vector<std::shared_ptr<Model>> children;
    void AddModel(std::shared_ptr<Model> model, float center[3]);
};