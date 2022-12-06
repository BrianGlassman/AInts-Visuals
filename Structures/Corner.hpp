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

// Model, not Structure, because Corners are part of Tunnels
class Corner : public Model
{
public:
    Corner();

	// Transformation to put the corner in the right spot
	Vector3 baseScale = {1, 1, 1};

	// FIXME make private once auto-generation is implemented // NORELEASE
    Surroundings surroundings;

	float radius = 0.1f;
    int n = 8; // Must be a multiple of 8 to handle 45 degree triple-corners
	// Number of strips making up each arm
	int panels = 4;

	void Create();
    void Draw(bool hasControl = true);
private:
	// Transformation to align the mesh properly
	Vector3 meshRotate = {0, 0, 0};
	Vector3 meshScale = {1, 1, 1};

	// Flag to get vertex winding correct
	bool windCW = false;
	GLenum windMode = GL_CCW;

	// Using multiple index arrays, so have to track the separations
	std::vector<int> indexBounds;

    void UpdateConnections();
	void SetRotateAndScale();

	void CreateClosed();
	void CreateCoreOuter(int i0, int i1, float c1, int i2, float c2);
	void CreateArm(int i0, int i1, int i2, bool reverse);
	void XTunnel(bool makeY, bool makeZ);
	void YTunnel(bool makeZ);
	void ZTunnel();

	void DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals); // FIXME pass by reference // NORELEASE
};