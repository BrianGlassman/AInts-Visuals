#pragma once

#include <vector>

class Noise
{
public:
    Noise();
    Noise(unsigned int seed);

    std::vector<float> getNoise(float x, float y, float z);
    std::vector<float> getNoise(std::vector<float> coords);

    float getFloat();
    std::vector<float> getVec3();
protected:
    std::vector<float> randomVec3();

    static bool initialized;
    static unsigned int seed;
};