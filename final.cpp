/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "final.hpp"
#include "display.hpp"
#include "input.hpp"
#include "util.hpp"
#include "window.hpp"
#include "textures.hpp"

#include "Colony.hpp"

Model* displayModelPtr;


#define LIGHT_SRC GL_LIGHT0
#define AMBIENT GL_LIGHT1
float ambient_level = 0.2;
float diffuse_level = 0.45;
float specular_level = 0.7;
#define SHINY_DEFAULT 10
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

static void init(int argc, char* argv[])
{
	// Do all the OpenGL setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	
	// Window
	CreateWindow();
	
	#ifdef USEGLEW
		if (glewInit() != GLEW_OK)
		{
			Fatal(1, "Error initializing GLEW\n");
		}
	#endif

	// Textures
	// FIXME for SOME REASON having this load here makes the one inside textures.cpp work
	LoadTexBMP("Textures/dirtGroundTileable.bmp");
	InitTextures();
	BindTexture("dirt"); // FIXME

	// Lighting
	initLighting();

	// Enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	ErrCheck("init");
}

void SetCallbacks()
{
	glutDisplayFunc(display);
	//FIXME
	glutReshapeFunc(reshape);
	glutVisibilityFunc(visible);

	SetInputCallbacks();

	ErrCheck("SetCallbacks");
}

void PopulateColony(Colony& colony)
{
	float tunnelCoords[][3] = {
		// Base layer
		{0, 0, 0},
		{0, 0, 1},
		{0, 0, 2},
		{1, 0, 2},

		// Up shaft
		{0, 1, 0},
		{0, 2, 0},
		{0, 3, 0},
		{-1, 3, 0},
	};

	unsigned char f = 1;
	unsigned char b = 2;
	unsigned char u = 4;
	unsigned char d = 8;
	unsigned char l = 16;
	unsigned char r = 32;

	unsigned char tunnelCon[] = {
		f+r+l+u,
		f+b+l,
		b+l+d,
		b+r,

		u+d,
		u+d+l,
		d+r,
		u+l,
	};

	float chamberCoords[][3] = {
		// Individual chambers
		{-1, 0, 0},
		{1, 2, 0},
		{0, -1, 2},

		// Grouped chambers
		{1, 0, 0},
		{2, 0, 0},
		{1, 0, 1},
		{2, 0, 1},
	};

	for (int i = 0; i < 8; i++)
	{
		colony.AddTunnel(tunnelCoords[i], tunnelCon[i]);
	}
	for (auto& i : chamberCoords)
	{
		colony.AddChamber(i);
	}

	colony.AddHill(-1, 4, 0);

	colony.center[1] = -1;
}

int main(int argc, char* argv[])
{
	// Create the colony to be displayed
	Colony colony;
	if (true)
		PopulateColony(colony);
	else
	{
		float coords[] = {0, 0, 0};
		colony.AddTunnel(coords, 16);
	}
	displayModelPtr = &colony;

	init(argc, argv);

	SetCallbacks();

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	ErrCheck("main");
	glutMainLoop();
	
	fprintf(stdout, "Passed loop\n");
	return 99;
}