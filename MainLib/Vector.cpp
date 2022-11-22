#include "CSCIx229.h"

#include "Vector.hpp"

float& Vector3::operator[](int index)
{
    switch(index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default:
            Fatal(999, "Invalid index %d\n", index);
            return x; // Have to return something or compiler grumbles
    }
}

std::vector<float> Vector3::asStdVec()
{
    return std::vector<float> ({x, y, z});
}

std::vector<int> Vector3Int::asStdVec()
{
    return std::vector<int> ({x, y, z});
}