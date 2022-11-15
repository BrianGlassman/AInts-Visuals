#include <iostream>

#include "Noise.hpp"

int main(int argc, char* argv[])
{
    Noise noise;

    auto vec = noise.getVec3();
    fprintf(stdout, "%f\n", noise.getFloat());
    fprintf(stdout, "(%f, %f, %f)\n", vec[0], vec[1], vec[2]);

    return 0;
}