#pragma once

#include <vector>

#include "Vector.hpp"

class Vertex
{
public:
    Vertex(int idx) { this->idx = idx; }
    Vertex(int idx, Vector3 coords) : Vertex(idx) { this->coords = coords; }

    Vector3 coords;
    int idx;
    std::vector<int> neighbors;

    float x() { return coords.x; }
    float y() { return coords.y; }
    float z() { return coords.z; }

    void AddNeighbor(int neighbor, bool debug = false);
    void RemoveNeighbor(int neighborToRemove, bool debug = false);
    void ReplaceNeighbor(int oldN, int newN, bool debug);
};