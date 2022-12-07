#pragma once

#include <memory>

#include "Chamber.hpp"
#include "Objects.hpp"

class MinePiece : public Chamber
{
public:
    MinePiece(std::shared_ptr<OBJ> OBJtoUse);

    void PreCreate() override;
    void Create(bool joinToChamber);
    void Create() override { Create(false); }
    void Draw(bool hasControl = true) override;
private:
    std::vector<int> OBJindices;
    std::vector<Vector3> OBJcolors;
    std::shared_ptr<OBJ> OBJptr;

    void DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals);
};