#pragma once

#include <vector>
#include <memory>

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Colony : public Model
{
public:
    Colony();
    void Draw();
    void AddTunnel(float center[3]);
    void AddTunnel(float x, float y, float z);
    void AddChamber(float center[3]);
    void AddChamber(float x, float y, float z);
    void AddHill(float center[3]);
    void AddHill(float x, float y, float z);
private:
    std::vector<std::shared_ptr<Model>> children;
    void AddModel(std::shared_ptr<Model> model, float center[3]);
};