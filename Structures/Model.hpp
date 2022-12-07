#pragma once

#include <vector>

#include "globals.hpp"
#include "Vector.hpp"
#include "Vertex.hpp"

// These aren't strictly needed here, but all the CPPs need them
#include "util.hpp" 
#include "Noise.hpp"

// Abstract class
class Model
{
public:
	Model();
	virtual ~Model(); // Need a virtual destructor for polymorphics
	StructureType type = StructureType::NONE;
	
	Vector3 center = {0.0f, 0.0f, 0.0f};
	bool created = false;

	// OPTIM should find a way to get rid of this // NORELEASE
	std::vector<Vertex>* getCL() { Fatal(999, "Called Model's getCL\n"); return NULL; }

	virtual void PreCreate();
    virtual void Create();
	virtual void PostCreate();
	void DrawNormals(float scale = 1);
	virtual void Draw(bool hasControl = true) = 0; // pure virtual

    virtual void ApplyNoise();
protected:
	// Scaling factor applied to the noise effect
	float noiseScale;

    std::vector<Vector3> vertices;
	std::vector<Vector3> baseVertices;
	std::vector<Vector3> normals;
	std::vector<Vector3> baseNormals;
	std::vector<std::vector<float>> texCoords;
	std::vector<int> indices;

	void ApplyNoiseHelper(unsigned int i, float scale);
	void DrawNormalHelper(float scale, std::vector<Vector3> verticesToUse, std::vector<Vector3> normalsToUse);
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