#pragma once

#include "Chamber.hpp"
#include "MinePiece.hpp"
#include "Objects.hpp"

class Mine : public Chamber
{
public:
    Mine();

    void PreCreate() override;
    void Create() override;
    void ApplyNoise() override;
    void Draw(bool hasControl = true) override;
private:
    std::vector<MinePiece> pieces;

    // void DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals);
};