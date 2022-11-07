#include "lighting.hpp"
#include "globals.hpp"

#define SHINY_DEFAULT 10

int IDs[8] = {
	GL_LIGHT0,
	GL_LIGHT1,
	GL_LIGHT2,
	GL_LIGHT3,
	GL_LIGHT4,
	GL_LIGHT5,
	GL_LIGHT6,
	GL_LIGHT7,
};
int nextID = 0;

class Light
{
public:
	Light(float ambient = 0, float diffuse = 0, float specular = 0);

	void SetAmbientLevel(float level);
	void SetDiffuseLevel(float level);
	void SetSpecularLevel(float level);
protected:
	int id;
	float position[3] = {0, 0, 0};

	void SetLightfv(GLenum pname, float level);
};

Light::Light(float ambient, float diffuse, float specular)
{
	// Assign an ID
	if (nextID > 7) Fatal(999, "Ran out of lights\n");
	id = IDs[nextID];
	nextID++;

	// Setup the light
	glEnable(id);

	// Translate intensity to color vectors
	SetAmbientLevel(ambient);
	SetDiffuseLevel(diffuse);
	SetSpecularLevel(specular);
}

void Light::SetLightfv(GLenum pname, float level)
{
	float vec[] = {level, level, level, 1.0};
	glLightfv(id, pname, vec);
}
void Light::SetAmbientLevel(float level)
{
	SetLightfv(GL_AMBIENT, level);
}
void Light::SetDiffuseLevel(float level)
{
	SetLightfv(GL_DIFFUSE, level);
}
void Light::SetSpecularLevel(float level)
{
	SetLightfv(GL_SPECULAR, level);
}

enum class OrbitMode {
	orbiting,
	manual,
};

class OrbitLight : Light
{
public:
	OrbitLight(float ambient = 0, float diffuse = 0, float specular = 0);

	float speed = 1.0;
	float radius = 5;

	void DrawLight();
private:
	float azimuth, elevation;

	OrbitMode mode = OrbitMode::orbiting;
};

OrbitLight::OrbitLight(float ambient, float diffuse, float specular)
{
	// FIXME constructor chaining needed
	{
		// Assign an ID
		if (nextID > 7) Fatal(999, "Ran out of lights\n");
		id = IDs[nextID];
		nextID++;

		// Setup the light
		glEnable(id);

		// Translate intensity to color vectors
		SetAmbientLevel(ambient);
		SetDiffuseLevel(diffuse);
		SetSpecularLevel(specular);
	}


	// FIXME temporary hack
	float position[] = {baseMag, 0, 0, 0};
	glLightfv(id, GL_POSITION, position);
}

void initLighting()
{
	// Setup for Lighting, based on Ex. 13
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_NORMALIZE);

	// Use eye point for specular reflection
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0); 

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, SHINY_DEFAULT);

	// Setting color will set ambient and diffuse colors
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	Light ambient(0.0, 0, 0);
	OrbitLight orbiter(0, 0.45, 0.7);

	ErrCheck("initLighting");
}

void OrbitLight::DrawLight()
{
	// Handle the light source
	if (mode == OrbitMode::orbiting) azimuth -= speed;
	azimuth = fmod(azimuth, 360.0);
	float position[] = {
		radius*Cos(azimuth)*Cos(elevation),
		radius*Sin(elevation),
		radius*Sin(azimuth)*Cos(elevation),
	1.0};
	glLightfv(id, GL_POSITION, position);
	
	// Draw gizmos
	glPushMatrix(); {
		glTranslatef(position[0], position[1], position[2]);

		glColor3f(1, 1, 1);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES); {
			// Draw an unlighted cross at the light source
			glVertex3d( 0, -1,  0);
			glVertex3d( 0, +1,  0);
			glVertex3d(-1,  0,  0);
			glVertex3d(+1,  0,  0);
			glVertex3d( 0,  0, -1);
			glVertex3d( 0,  0, +1);
			
			// Draw axis projections to the origin
			glColor3f(0.6, 0, 0);
			glVertex3d(0, 0, 0);
			glVertex3d(-position[0], 0, 0);
			glColor3f(0, 0, 0.7);
			glVertex3d(-position[0], 0, 0);
			glVertex3d(-position[0], 0, -position[2]);
			glColor3f(0, 0.5, 0);
			glVertex3d(-position[0], 0, -position[2]);
			glVertex3d(-position[0], -position[1], -position[2]);
		} glEnd();
		glEnable(GL_LIGHTING);
	} glPopMatrix();
}