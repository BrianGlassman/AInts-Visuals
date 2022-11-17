#pragma once

#include "CSCIx229.h"
#include "Structure.hpp"
#include "Corner.hpp"
#include "util.hpp" // Model class

class Tunnel : public Structure
{
public:
	Tunnel();
	// FIXME temporary convenience until auto-generating is implemented
	Tunnel(unsigned char sides);
	void Create();
	void ApplyNoise(Noise* noise, float offset[]);
	void Draw();

	float radius = 0.2f;
	int n = 8; // Must be a multiple of 8 for the sake of passing to Corner

	bool   right = true; // +X
	bool    left = true; // -X
	bool     top = true; // +Y
	bool  bottom = true; // -Y
	bool forward = true; // +Z
	bool    back = true; // -Z
private:
	Corner corners[8];

	void OldDraw();
};