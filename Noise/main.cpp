#include <iostream>

#include "Noise.hpp"
#include "Perlin.hpp"

int main(int argc, char* argv[])
{
    Noise* noisePtr = new Perlin;

    auto vec = noisePtr->getVec3();
    fprintf(stdout, "float: %f\n", noisePtr->getFloat());
    fprintf(stdout, "vec3: (%f, %f, %f)\n", vec[0], vec[1], vec[2]);
    vec = noisePtr->getNoise(0, 0, 0);
    fprintf(stdout, "noise at (0, 0, 0): (%f, %f, %f)\n", vec[0], vec[1], vec[2]);
    vec = noisePtr->getNoise(1, 2, 3);
    fprintf(stdout, "noise at (1, 2, 3): (%f, %f, %f)\n", vec[0], vec[1], vec[2]);
    vec = noisePtr->getNoise(0, 0, 0);
    fprintf(stdout, "again at (0, 0, 0): (%f, %f, %f)\n", vec[0], vec[1], vec[2]);
    vec = noisePtr->getNoise(0.5, 0.5, 0.5);
    fprintf(stdout, "noise at (0.5, 0.5, 0.5): (%f, %f, %f)\n", vec[0], vec[1], vec[2]);

    delete noisePtr;

    return 0;
}