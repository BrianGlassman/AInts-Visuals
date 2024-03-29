#pragma once

#include "Noise.hpp"
#include "Vector.hpp"

class Perlin : public Noise
{
public:
    Perlin() : Noise() {};
    Perlin(unsigned int seed) : Noise(seed) {};

    std::vector<float> getNoise(float x, float y, float z);

    Vector3 getPVector(Vector3Int coords);
    Vector3 getPVector(int x, int y, int z);

    void DrawNoise();
protected:
    //FIXME HACK. Tried using unordered_map but it got weird about the key. Seems solvable, though
    std::vector<Vector3Int> pVectorsKeys;
    std::vector<Vector3> pVectorsVals;
};