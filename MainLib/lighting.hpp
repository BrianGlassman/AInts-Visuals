#pragma once

#include "CSCIx229.h"

void initLighting();

class Light
{
public:
	Light(float ambient = 0, float diffuse = 0, float specular = 0);

	int GetConst()
	{
		switch (id)
		{
		case GL_LIGHT0:
			return 0;
		case GL_LIGHT1:
			return 1;
		case GL_LIGHT2:
			return 2;
		case GL_LIGHT3:
			return 3;
		case GL_LIGHT4:
			return 4;
		case GL_LIGHT5:
			return 5;
		case GL_LIGHT6:
			return 6;
		case GL_LIGHT7:
			return 7;
		default:
			Fatal(999, "Unrecognized light id");
			return -1;
		}
	}
	void Toggle();
	void SetAmbientLevel(float level);
	void SetDiffuseLevel(float level);
	void SetSpecularLevel(float level);
	void Increment() { Crement( 0.1); }
	void Decrement() { Crement(-0.1); }
protected:
	GLenum id;
	bool active;
	float position[4] = {0, 0, 0, 0}; // MUST be a float[4] (supposedly)

	float ambient; bool ambientActive;
	float diffuse; bool diffuseActive;
	float specular; bool specularActive;

	void Crement(float val); // Called by Increment/Decrement
	void SetLightfv(GLenum pname, float level);
};

class OrbitLight : public Light
{
public:
	OrbitLight(float ambient = 0, float diffuse = 0, float specular = 0);

	float speed = 1.0;
	float radius = 5;

    void UpdatePosition(float dAz, float dEl);
	void UpdatePosition() { UpdatePosition(0, 0); }
	void Draw();
private:
	float azimuth = 0, elevation = 0;
};

namespace Globals::Lighting {
	extern Light* ambient;
	extern OrbitLight* directed;
}