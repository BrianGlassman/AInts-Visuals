#pragma once

#include <vector>

#include "Vector.hpp"

class Vertex
{
public:
    Vector3 coords;
    std::vector<Vertex*> neighbors;

    float x() { return coords.x; }
    float y() { return coords.y; }
    float z() { return coords.z; }
};