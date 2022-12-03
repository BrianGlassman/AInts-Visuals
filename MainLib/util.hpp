#pragma once

#include <vector>

#include "CSCIx229.h"

#include "Noise.hpp" // Noise class for ApplyNoise
#include "Vector.hpp"
#include "Vertex.hpp" // FIXME only needed for temp things

#define ORIGIN 0.0, 0.0, 0.0
#define X_AXIS 1.0, 0.0, 0.0
#define Y_AXIS 0.0, 1.0, 0.0
#define Z_AXIS 0.0, 0.0, 1.0

void Polar2Cart(float radius, float theta, float* x_out, float* y_out);

void DrawAxes(float scale);

void DrawLitQuad(const float A[], const float B[], const float C[], const float D[]);

// Abstract class
class Model
{
public:
	Model();
	virtual ~Model(); // Need a virtual destructor for polymorphics
	int type;
	
	Vector3 center = {0.0f, 0.0f, 0.0f};
	bool created = false;

	// FIXME should find a way to get rid of this
	std::vector<Vertex>* getCL() { Fatal(999, "Called Model's getCL\n"); return NULL; }

	virtual void PreCreate();
    virtual void Create();
	virtual void PostCreate();
	void DrawNormals(float scale = 1);
	virtual void Draw(bool hasControl = true);

    virtual void ApplyNoise();
protected:
	// Scaling factor applied to the noise effect
	float noiseScale;

    std::vector<Vector3> vertices;
	std::vector<Vector3> baseVertices;
	std::vector<Vector3> normals;
	std::vector<std::vector<float>> texCoords;
	std::vector<int> indices;
};

class Cylinder : public Model
{
public:
	Cylinder();

	bool top = true;
	bool sides = true;
	bool bottom = true;
	float radius = 0.5f;
	float height = 1.0f;
	int n = 12;

	void Draw(bool hasControl = true);
	void SetTopCenter(float x, float y, float z);
	void SetBottomCenter(float x, float y, float z);
};

class Cube : public Model
{
public:
	Cube();

	bool   right = true; // +X
	bool    left = true; // -X
	bool     top = true; // +Y
	bool  bottom = true; // -Y
	bool forward = true; // +Z
	bool    back = true; // -Z

	// Internal (i.e. center to wall) distance
	float radius = 0.5f;

	void Draw(bool hasControl = true);
private:
	unsigned char sides;
};

class Sphere : public Model
{
public:
	Sphere();

	float radius = 0.5f;
	int n = 12;

	void Draw(bool hasControl = true);
};