#pragma once

#include <memory>

#include "Structure.hpp"
#include "Colony.hpp"
#include "globals.hpp"

class BuildIndicator : public Structure
{
public:
    BuildIndicator();

    Colony* colony;

    void HandleKey(unsigned char k);
    void SetModel(StructureType type);
    void Create();
    void ApplyNoise();
    void Draw(bool hasControl = true);
private:
    std::shared_ptr<Structure> model;
};