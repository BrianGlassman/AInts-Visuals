#include "lighting.hpp"
#include "globals.hpp"
#include "Shaders.hpp"
#include "util.hpp"

#define SHINY_DEFAULT 10

GLenum IDs[8] = {
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

Light* Globals::Lighting::ambient;
OrbitLight* Globals::Lighting::directed;

void initLighting()
{
	// Setup for Lighting, based on Ex. 13
	glEnable(GL_LIGHTING);
	glShadeModel(Toggles::Light::smooth ? GL_SMOOTH : GL_FLAT);

	// Turn off the default ambient light
	glLightModeli(GL_LIGHT_MODEL_AMBIENT, 0);

	glEnable(GL_NORMALIZE);

	// Use eye point for specular reflection
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0); 

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, SHINY_DEFAULT);

	ErrCheck("initLighting");
}


Light::Light(float ambient, float diffuse, float specular)
{
	// Assign an ID
	if (nextID > 7) Fatal(999, "Ran out of lights\n");
	id = IDs[nextID];
	// printf("Creating light number %d, ID = %d\n", nextID, id); // NORELEASE
	nextID++;

	// Setup the light
	glEnable(id);
	active = true;

	// Translate intensity to color vectors
	SetAmbientLevel(ambient);
	SetDiffuseLevel(diffuse);
	SetSpecularLevel(specular);

	// fprintf(stdout, "(Creation) Light pos = (%f, %f, %f), elevation = %f\n", position[0], position[1], position[2]); // NORELEASE
}

void Light::Toggle()
{
	if (active) glDisable(id);
	else glEnable(id);
	active = !active;
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

OrbitLight::OrbitLight(float ambient, float diffuse, float specular) : Light(ambient, diffuse, specular)
{
}

void OrbitLight::UpdatePosition()
{
	// Position is set relative to the eye, so have to update whenever the eye point moves
	if (Toggles::Light::lightOrbiting) azimuth -= speed;
	azimuth = fmod(azimuth, 360.0);
	position[0] = radius*Cos(azimuth)*Cos(elevation);
	position[1] = radius*Sin(elevation);
	position[2] = radius*Sin(azimuth)*Cos(elevation);
	// fprintf(stdout, "Light pos = (%f, %f, %f), elevation = %f\n", position[0], position[1], position[2], elevation); // NORELEASE
	glLightfv(id, GL_POSITION, position);
}

void OrbitLight::Draw()
{
	PushShader(Shader::fixedPipeline);
	// Draw gizmos
	glPushMatrix(); {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		
		glTranslatef(position[0], position[1], position[2]);

		SetColor(1, 1, 1);
		glPushMatrix(); {
			glScalef(0.2, 0.2, 0.2);
			glBegin(GL_LINES); {
				// Draw an unlighted cross at the light source
				glVertex3d( 0, -1,  0);
				glVertex3d( 0, +1,  0);
				glVertex3d(-1,  0,  0);
				glVertex3d(+1,  0,  0);
				glVertex3d( 0,  0, -1);
				glVertex3d( 0,  0, +1);
			} glEnd();
		} glPopMatrix();

		glBegin(GL_LINES); { // NORELEASE
			// Draw axis projections to the origin
			SetColor(0.6, 0, 0);
			glVertex3d(0, 0, 0);
			glVertex3d(-position[0], 0, 0);
			SetColor(0, 0, 0.7);
			glVertex3d(-position[0], 0, 0);
			glVertex3d(-position[0], 0, -position[2]);
			SetColor(0, 0.5, 0);
			glVertex3d(-position[0], 0, -position[2]);
			glVertex3d(-position[0], -position[1], -position[2]);
		} glEnd();

		SetColor(1, 1, 1);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	} glPopMatrix();
	PopShader();

	ErrCheck("OrbitLight::Draw");
}
