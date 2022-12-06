#pragma once

#include "Chamber.hpp"

class Mine : public Chamber
{
public:
    Mine();

    void PreCreate() override;
    void Create() override;
    void Draw(bool hasControl = true) override;
private:
    std::vector<int> OBJindices;
    std::vector<Vector3> OBJcolors;

    void DrawHelper(std::vector<Vector3> drawVertices);
};