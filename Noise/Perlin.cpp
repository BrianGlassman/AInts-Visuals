#include <algorithm>

#include "Perlin.hpp"

Vector3 Perlin::getPVector(Vector3Int coords)
{
    // https://www.geeksforgeeks.org/how-to-find-index-of-a-given-element-in-a-vector-in-cpp/
    // https://stackoverflow.com/a/69070581
    auto search = [&](const Vector3Int& key) {
        return (key.x == coords.x) && (key.y == coords.y) && (key.z == coords.z);
    };
    auto iter = std::find_if(pVectorsKeys.begin(), pVectorsKeys.end(), search);
    if (iter == pVectorsKeys.end())
    {
        // Create, store, and return a new vector
        Vector3 vec({coords.x*2.0, coords.y*2.0, coords.z*2.0}); // FIXME should be random and such
        pVectorsKeys.push_back(coords);
        pVectorsVals.push_back(vec);
        return vec;
    }
    else
    {
        int key = iter - pVectorsKeys.begin();
        return pVectorsVals[key];
    }
}
Vector3 Perlin::getPVector(int x, int y, int z)
{
    Vector3Int coords({x, y, z});
    return getPVector(coords);
}

std::vector<float> Perlin::getNoise(float x, float y, float z)
{
    // FIXME interpolate
    Vector3 vec3 = getPVector(x, y, z);
    std::vector<float> vec;
    vec.push_back(vec3.x); vec.push_back(vec3.y); vec.push_back(vec3.z);
    return vec;
}