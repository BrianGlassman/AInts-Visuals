#pragma once

#include <memory>

#include "Structure.hpp"

class BuildIndicator : public Structure
{
public:
    BuildIndicator();

    void Draw();
private:
    std::unique_ptr<Structure> model;
};