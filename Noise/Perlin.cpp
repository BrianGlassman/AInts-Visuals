#include <algorithm>
#include <cmath>

#include <iostream> // Just for debugging printouts

#include "CSCIx229.h"
#include "Perlin.hpp"
#include "Shaders.hpp"
#include "util.hpp"

// Broadly adapted from https://cs.nyu.edu/~perlin/noise/

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

float PerlinFade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}
float _BezierY(float t)
{ // Cubic Bezier curve
    float t2 = t*t, t3 = t*t*t;
    float it = 1 - t;
    return 3*it*t2 + t3;
}
float _BezierX(float t)
{ // Cubic Bezier curve
    float P0 = 0;
    float P1 = 0.25;
    float P2 = 1 - P1;
    float P3 = 1;
    float t2 = t*t, t3 = t*t*t;
    float it = 1 - t;
    float it2 = it*it;
    float it3 = it*it*it;
    return it3*P0 + 3*it2*t*P1 + 3*it*t2*P2 + t3*P3;
}
float BezierFade(float x)
{ // Iterate to find the right Y value for the given X
    float t = x;
    float xCalc;
    int i = 0;
    while (i < 100)
    {
        xCalc = _BezierX(t);
        if (abs(xCalc - x) < 1e-6)
            break;
        t = t - (xCalc - x);
        i++;
    }
    if (i == 100) Fatal(999, "Max iterations reached for x = %f\n", x);
    return _BezierY(t);
}
float fade(float x)
{
    return BezierFade(x);
}

float lerp(float fraction, float low, float high)
{
    return low + fraction * (high - low);
}
Vector3 lerp(float fraction, Vector3 low, Vector3 high)
{
    // Need fade, otherwise there's sharp angles at cell borders
    auto x = lerp(fade(fraction), low.x, high.x);
    auto y = lerp(fade(fraction), low.y, high.y);
    auto z = lerp(fade(fraction), low.z, high.z);
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

    std::vector<float> vec;
    vec.push_back(xyz.x); vec.push_back(xyz.y); vec.push_back(xyz.z);
    return vec;
}

void Perlin::DrawNoise()
{
    PushShader(Shader::fixedPipeline);
    glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_LINE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glPointSize(15);
    glLineWidth(7);
    SetColor(1, 1, 0.6, 0.5);

    for (unsigned int i = 0; i < pVectorsKeys.size(); i++)
    {
        auto coords = pVectorsKeys[i];
        glBegin(GL_POINTS);
        glVertex3f(coords.x, coords.y, coords.z);
        glEnd();

        auto p = pVectorsVals[i];
        glBegin(GL_LINES);
        glVertex3f(coords.x, coords.y, coords.z);
        glVertex3f(
            coords.x + p.x*0.6,
            coords.y + p.y*0.6,
            coords.z + p.z*0.6);
        glEnd();
    }

    SetColor(1, 1, 1, 1);
    glPopAttrib();
    PopShader();
}