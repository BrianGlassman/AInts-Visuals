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

    void AddNeighbor(int neighbor, bool debug = false)
    {
        neighbors.push_back(neighbor);
        // TODO check for duplicate links
        if (debug) printf("Linking %d to %d\n", idx, neighbor);
    }
    void RemoveNeighbor(int neighborToRemove, bool debug = false)
    {
        for (unsigned int i = 0; i < neighbors.size(); i++)
        {
            if (neighbors[i] == neighborToRemove)
            {
                if (debug) printf("In %d: Removing %d\n", idx, neighborToRemove);
                neighbors.erase(neighbors.begin() + i);
                return;
            }
        }
        Fatal(999, "Trying to remove neighbor that doesn't exist\n");
    }
    void ReplaceNeighbor(int oldN, int newN, bool debug = false)
    {
        for (unsigned int i = 0; i < neighbors.size(); i++)
        {
            if (neighbors[i] == oldN)
            {
                if (debug) printf("In %d: Replacing %d with %d\n", idx, oldN, newN);
                neighbors[i] = newN;
                return;
            }
        }
        Fatal(999, "Trying to replace neighbor (%d) that doesn't exist\n", oldN);
    }
};