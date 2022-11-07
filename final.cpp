/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "final.hpp"
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "lighting.hpp"
#include "util.hpp"
#include "window.hpp"
#include "textures.hpp"

#include "Colony.hpp"

Model* displayModelPtr;

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

	// Wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	// Face culling and Z-buffering
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
	unsigned char r = 16;
	unsigned char l = 32;

	unsigned char tunnelCon[] = {
		f+r+l+u,
		f+b+r,
		b+r+d,
		b+l,

		u+d,
		u+d+r,
		d+l,
		u+r,
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

void PopulateTunnels(Colony& colony)
{
	float tunnelCoords[][3] = {
		// 0
		{0, 0, 0},

		// 1
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
		{-1, 0, 0},
		{0, -1, 0},
		{0, 0, -1},

		// 2 - x = 0
		{0, 1, 1},
		{0, -1, 1},
		{0, 1, -1},
		{0, -1, -1},

		// 2 - y = 0
		{1, 0, 1},
		{-1, 0, 1},
		{1, 0, -1},
		{-1, 0, -1},

		// 2 - z=0
		{1, 1, 0},
		{-1, 1, 0},
		{1, -1, 0},
		{-1, -1, 0},

		// 3 - all +
		{1, 1, 1},

		// 3 - one -
		{-1, 1, 1},
		{1, -1, 1},
		{1, 1, -1},

		// 3 - two -
		{-1, -1, 1},
		{-1, 1, -1},
		{1, -1, -1},

		// 3 - all -
		{-1, -1, -1},

		// FIXME doesn't include 2 tunnels per axis
	};

	unsigned char f = 1;
	unsigned char b = 2;
	unsigned char u = 4;
	unsigned char d = 8;
	unsigned char r = 16;
	unsigned char l = 32;

	unsigned char tunnelCon[] = {
		// 0
		0,

		// 1
		r,
		u,
		f,
		l,
		d,
		b,

		// 2 - x = 0
		u+f,
		d+f,
		u+b,
		d+b,

		// 2 - y = 0
		r+f,
		l+f,
		r+b,
		l+b,

		// 2 - z=0
		r+u,
		l+u,
		r+d,
		l+d,

		// 3 - all +
		r+u+f,

		// 3 - one -
		l+u+f,
		r+d+f,
		r+u+b,

		// 3 - two -
		l+d+f,
		l+u+b,
		r+d+b,

		// 3 - all -
		l+d+b,
	};

	for (int i = 0; i < 27; i++)
	{
		colony.AddTunnel(tunnelCoords[i], tunnelCon[i]);
	}
}

int main(int argc, char* argv[])
{
	// Create the scene to be displayed
	Scene sceneChoice = Scene::colony;
	Colony colony;
	Colony tunnel;
	switch(sceneChoice)
	{
	case Scene::colony:
	{
		baseMag = 4;
		PopulateColony(colony);
		displayModelPtr = &colony;
		break;
	}
	case Scene::tunnel:
	{
		baseMag = 1;
		float coords[] = {0, 0, 0};
		// Looking just at default (white) corner: all good
		tunnel.AddTunnel(coords, 63);
		displayModelPtr = &tunnel;
		break;
	}
	case Scene::allTunnels:
	{
		baseMag = 4;
		PopulateTunnels(tunnel);
		displayModelPtr = &tunnel;
		break;
	}
	default:
		Fatal(999, "Unknown scene\n");
	}

	init(argc, argv);

	// Enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	SetCallbacks();

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}