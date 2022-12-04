#pragma once

#include "CSCIx229.h"
#include "Structure.hpp"
#include "Corner.hpp"
#include "util.hpp" // Model class

class Tunnel : public Structure
{
public:
	Tunnel();

	void Create();
	void ApplyNoise();
	void Draw(bool hasControl);

	float radius = 0.1f;
	int n = 8; // Must be a multiple of 8 for the sake of passing to Corner
private:
	Corner corners[8];

	void CreateCLHelper(std::vector<Vertex> &CLtoUse, const int axis, const bool flip);
	void CreateCenterline(int axis, bool flip);

	void OldDraw();
};