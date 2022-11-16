#include <algorithm>
#include <cmath>

#include <iostream> // Just for debugging printouts

#include "Perlin.hpp"

// Some functions drawn from https://cs.nyu.edu/~perlin/noise/

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

        // Create a random vector
        Vector3 vec({getFloat(), getFloat(), getFloat()});

        // Normalize
        float mag = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
        vec.x /= mag; vec.y /= mag; vec.z /= mag;

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

float lerp(float fraction, float low, float high)
{
    return low + fraction * (high - low);
}
Vector3 lerp(float fraction, Vector3 low, Vector3 high)
{
    auto x = lerp(fraction, low.x, high.x);
    auto y = lerp(fraction, low.y, high.y);
    auto z = lerp(fraction, low.z, high.z);
    Vector3 ret({x, y, z});
    return ret;
}

std::vector<float> Perlin::getNoise(float x, float y, float z)
{
    // Get the cube corners and corresponding vectors
    int xLow = floor(x), yLow = floor(y), zLow = floor(z);
    int xHigh = xLow + 1, yHigh = yLow + 1, zHigh = zLow + 1;
    Vector3 lll = getPVector( xLow,  yLow,  zLow);
    Vector3 lhl = getPVector( xLow, yHigh,  zLow);
    Vector3 llh = getPVector( xLow,  yLow, zHigh);
    Vector3 lhh = getPVector( xLow, yHigh, zHigh);
    Vector3 hll = getPVector(xHigh,  yLow,  zLow);
    Vector3 hhl = getPVector(xHigh, yHigh,  zLow);
    Vector3 hlh = getPVector(xHigh,  yLow, zHigh);
    Vector3 hhh = getPVector(xHigh, yHigh, zHigh);

    // Find relative X, Y, Z of point in cube
    x -= xLow; y -= yLow; z -= zLow;

    //--- Interpolate ---
    // X axis
    auto xll = lerp(x, lll, hll);
    auto xhl = lerp(x, lhl, hhl);
    auto xlh = lerp(x, llh, hlh);
    auto xhh = lerp(x, lhh, hhh);

    // Y axis
    auto xyl = lerp(y, xll, xhl);
    auto xyh = lerp(y, xlh, xhh);

    // Z axis
    auto xyz = lerp(z, xyl, xyh);
    //---

    float mag = sqrt(xyz.x*xyz.x + xyz.y*xyz.y + xyz.z*xyz.z);

    std::vector<float> vec;
    vec.push_back(xyz.x); vec.push_back(xyz.y); vec.push_back(xyz.z);
    return vec;
}