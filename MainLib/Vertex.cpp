#include "Vertex.hpp"
#include "CSCIx229.h"

void Vertex::AddNeighbor(int neighbor, bool debug)
{
    neighbors.push_back(neighbor);
    // TODO check for duplicate links
    if (debug) printf("Linking %d to %d\n", idx, neighbor);
}

void Vertex::RemoveNeighbor(int neighborToRemove, bool debug)
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

void Vertex::ReplaceNeighbor(int oldN, int newN, bool debug)
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