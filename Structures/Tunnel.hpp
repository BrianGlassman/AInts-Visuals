#pragma once

#include "CSCIx229.h"
#include "Structure.hpp"
#include "Corner.hpp"
#include "util.hpp" // Model class

class Tunnel : public Structure
{
public:
	Tunnel() : Tunnel(63) {}
	// FIXME temporary convenience until auto-generating is implemented
	Tunnel(unsigned char sides);

	void Create();
	void ApplyNoise(float offset[]);
	void Draw();

	float radius = 0.1f;
	int n = 8; // Must be a multiple of 8 for the sake of passing to Corner
private:
	Corner corners[8];

	void CreateCLHelper(std::vector<Vertex> &CLtoUse, const int axis, const bool flip);
	void CreateCenterline(int axis, bool flip);

	void OldDraw();
};