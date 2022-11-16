#pragma once

#include "Noise.hpp"

struct Vector3Int {
    int x;
    int y;
    int z;
};

struct Vector3 {
    float x;
    float y;
    float z;
};

class Perlin : public Noise
{
public:
    Perlin();
    Perlin(unsigned int seed);

    std::vector<float> getNoise(float x, float y, float z);

    Vector3 getPVector(Vector3Int coords);
    Vector3 getPVector(int x, int y, int z);
protected:
    //FIXME HACK. Tried using unordered_map but it got weird about the key. Seems solvable, though
    std::vector<Vector3Int> pVectorsKeys;
    std::vector<Vector3> pVectorsVals;
};