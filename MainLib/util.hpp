#pragma once

#include <vector>

#include "CSCIx229.h"

#define ORIGIN 0.0, 0.0, 0.0
#define X_AXIS 1.0, 0.0, 0.0
#define Y_AXIS 0.0, 1.0, 0.0
#define Z_AXIS 0.0, 0.0, 1.0

void Polar2Cart(float radius, float theta, float* x_out, float* y_out);

void DrawAxes(float scale);

void DrawLitQuad(const float A[], const float B[], const float C[], const float D[]);
