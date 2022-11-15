#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Noise.hpp"

bool Noise::initialized = false;
unsigned int Noise::seed = 0;

Noise::Noise(unsigned int seed)
{
    if (initialized)
    {
        fprintf(stderr, "Rand already seeded\n");
        exit(1);
    }
    else
    {
        std::srand(seed);
        this->seed = seed;
        fprintf(stdout, "Initializing rand with seed = %d\n", seed);
    }
}
Noise::Noise()
{ // Default constructor, use current time as seed if not already seeded
    if (initialized)
    {
        fprintf(stdout, "Re-using existing random seed");
    }
    else
    {
        Noise(time(NULL));
    }
}

Noise::~Noise() {}

float Noise::getFloat()
{
    float mn = -1.0, mx = 1.0;

    float raw = (float)rand() / RAND_MAX;
    float scaled = raw * (mx - mn);
    float centered = scaled - (mx - mn) / 2;
    return centered;
}

std::vector<float> Noise::getVec3()
{
    std::vector<float> vec({getFloat(), getFloat(), getFloat()});
    return vec;
}

std::vector<float> Noise::getNoise(float x, float y, float z)
{
    return getVec3();
}

std::vector<float> Noise::getNoise(std::vector<float> coords)
{
    return getNoise(coords[0], coords[1], coords[2]);
}