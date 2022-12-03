#pragma once

#include <memory>

#include "Structure.hpp"
#include "globals.hpp"

class BuildIndicator : public Structure
{
public:
    BuildIndicator();

    void HandleKey(unsigned char k);
    void SetModel(StructureType type);
    void Create();
    void Draw();
private:
    std::unique_ptr<Structure> model;
};