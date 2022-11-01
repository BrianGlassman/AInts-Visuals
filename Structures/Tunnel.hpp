#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Tunnel : public Model
{
public:
	Tunnel();
	// FIXME temporary convenience until auto-generating is implemented
	Tunnel(unsigned char sides);
	void Draw();

	float radius = 0.1f;

	bool   right = true; // +X
	bool    left = true; // -X
	bool     top = true; // +Y
	bool  bottom = true; // -Y
	bool forward = true; // +Z
	bool    back = true; // -Z
};