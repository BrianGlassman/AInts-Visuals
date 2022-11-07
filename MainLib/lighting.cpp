#include "lighting.hpp"

#define LIGHT_SRC GL_LIGHT0
#define AMBIENT GL_LIGHT1
#define SHINY_DEFAULT 10

float ambient_level = 0.2;
float diffuse_level = 0.45;
float specular_level = 0.7;

void initLighting()
{
	// Setup for Lighting, based on Ex. 13
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	// Translate intensity to color vectors
	float Ambient[] = {ambient_level, ambient_level, ambient_level, 1.0};
	float Diffuse[] = {diffuse_level, diffuse_level, diffuse_level, 1.0};
	float Specular[] = {specular_level, specular_level, specular_level, 1.0};

	glEnable(GL_NORMALIZE);

	// Setup ambient lighting
	glEnable(AMBIENT);
	glLightfv(AMBIENT, GL_AMBIENT, Ambient);

	// Setup directed lighting
	glEnable(LIGHT_SRC);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0); // Use eye point for specular reflection
	glLightfv(LIGHT_SRC, GL_DIFFUSE, Diffuse);
	glLightfv(LIGHT_SRC, GL_SPECULAR, Specular);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, SHINY_DEFAULT);

	// Setting color will set ambient and diffuse colors
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	ErrCheck("initLighting");
}