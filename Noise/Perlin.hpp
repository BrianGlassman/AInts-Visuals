#pragma once

#include "Noise.hpp"

class Perlin : public Noise
{
public:
    std::vector<float> getNoise(float x, float y, float z);
};