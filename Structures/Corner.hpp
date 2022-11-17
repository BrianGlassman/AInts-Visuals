#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class

class Surroundings {
public:
	int x;
	int y;
	int z;
	int xy;
	int xz;
	int yz;

	int sqrMagnitude() {
		return x*x + y*y + z*z;
	}
};

class Corner : public Model
{
public:
    Corner();

	// Transformation to put the corner in the right spot
	float baseScale[3] = {1, 1, 1};

	// FIXME make public once auto-generation is implemented
    Surroundings surroundings;

	float radius = 0.1f;
    int n = 12;

	void Create();
    void Draw();
private:
	// Transformation to align the mesh properly
	float meshRotate[3] = {0, 0, 0};
	float meshScale[3] = {1, 1, 1};

	// Flag to get vertex winding correct
	bool windCW = false;
	GLenum windMode = GL_CCW;

    void UpdateConnections();
	void SetRotateAndScale();

	void CreateClosed();
	void CreateOneTunnel();
	void CreateTwoTunnel();
	void CreateThreeTunnel();
};