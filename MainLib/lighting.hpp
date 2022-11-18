#pragma once

#include "CSCIx229.h"

void initLighting();

class Light
{
public:
	Light(float ambient = 0, float diffuse = 0, float specular = 0);

	void SetAmbientLevel(float level);
	void SetDiffuseLevel(float level);
	void SetSpecularLevel(float level);
protected:
	int id;
	float position[4] = {0, 0, 0, 0}; // MUST be a float[4] (supposedly)

	void SetLightfv(GLenum pname, float level);
};

class OrbitLight : Light
{
public:
	OrbitLight(float ambient = 0, float diffuse = 0, float specular = 0);

	float speed = 1.0;
	float radius = 5;

    void UpdatePosition();
	void Draw();
private:
    enum class OrbitMode {
        orbiting,
        manual,
    };

	float azimuth = 0, elevation = 0;

	OrbitMode mode = OrbitMode::orbiting;
};
