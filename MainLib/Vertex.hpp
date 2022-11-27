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

    void AddNeighbor(Vertex* neighbor, bool reciprocal = true)
    {
        // Add link to neighbor
        neighbors.push_back(neighbor);
        // Link neighbor to self
        // if (reciprocal) neighbor->AddNeighbor(this, false);
    }
};