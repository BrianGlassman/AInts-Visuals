#pragma once

#include "CSCIx229.h"

// MainLib
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "lighting.hpp"
#include "textures.hpp"
#include "window.hpp"

// Structures
#include "Colony.hpp"

// Noise
#include "Noise.hpp"
#include "Perlin.hpp"

// Menus
#include "Menus.hpp"

// Shaders
#include "Shaders.hpp"

Colony* displayModelPtr;
Colony colony;
Colony tunnel;
Colony chamber;
Colony hill;
// Colony allTunnels;

Noise* noisePtr;

OrbitLight* orbiterPtr;

//-----------------------
// Function declarations
//-----------------------
// Init stuff
static void init(int argc, char* argv[]);
void display();
void SetCallbacks()
{
	glutDisplayFunc(display);
	//FIXME
	glutReshapeFunc(reshape);
	glutVisibilityFunc(visible);

	SetInputCallbacks();

	ErrCheck("SetCallbacks");
}

void SetDisplayPtr()
{
	switch(Globals::sceneChoice)
	{
	case Scene::colony:
	{
		displayModelPtr = &colony;
		break;
	}
	case Scene::tunnel:
	{
		displayModelPtr = &tunnel;
		break;
	}
	case Scene::chamber:
	{
		displayModelPtr = &chamber;
		break;
	}
	case Scene::hill:
	{
		displayModelPtr = &hill;
		break;
	}
	// case Scene::allTunnels:
	// {
	// 	displayModelPtr = &allTunnels;
	// 	break;
	// }
	default:
		Fatal(999, "Unknown scene %d\n", Globals::sceneChoice);
	}
}

void PopulateColony(Colony& colony)
{
	float tunnelCoords[][3] = {
		// Base layer
		{0, 0, 0}, // 0
		{0, 0, 1}, // 1
		{0, 0, 2}, // 2
		{1, 0, 2}, // 3

		// Up shaft
		{0, 1, 0}, // 4
		{0, 2, 0}, // 5
		{0, 3, 0}, // 6
		{-1, 3, 0}, // 7
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
		r, // FIXME should be u+r,
	};

	float chamberCoords[][3] = {
		// Individual chambers
		{-1, 0, 0}, // 8
		{1, 2, 0},  // 9
		{0, -1, 2}, // 10

		// Grouped chambers
		{1, 0, 0}, // 11
		{2, 0, 0}, // 12
		{1, 0, 1}, // 13
		{2, 0, 1}, // 14
	};

	unsigned char chamberCon[] = {
		// Individual chambers
		r, // 8
		l, // 9
		u, // 10

		// Grouped chambers
		r+l+f, // 11
		l+f, // 12
		r+l+f+b, // 13
		l+b, // 14
	};

	Vector3 coords;
	for (int i = 0; i < 8; i++)
	{
		coords = tunnelCoords[i];
		coords.y -= 1;
		colony.AddTunnel(coords, tunnelCon[i]);
	}
	for (int i = 0; i < 7; i++)
	{
		coords = chamberCoords[i];
		coords.y -= 1;
		colony.AddChamber(coords, chamberCon[i]);
	}


	// FIXME don't do this manually
	{// --- TUNNELS ---
	// 0
	colony.children[0]->endpointForward[1] = 1;
	colony.children[0]->endpointRight[1] = 11;
	colony.children[0]->endpointLeft[1] = 8;
	colony.children[0]->endpointTop[1] = 4;

	// 1
	colony.children[1]->endpointForward[1] = 2;
	colony.children[1]->endpointBack[1] = 0;
	colony.children[1]->endpointRight[1] = 13;

	// 2
	colony.children[2]->endpointBack[1] = 1;
	colony.children[2]->endpointRight[1] = 3;
	colony.children[2]->endpointBottom[1] = 10;

	// 3
	colony.children[3]->endpointBack[1] = 13;
	colony.children[3]->endpointLeft[1] = 2;

	// 4
	colony.children[4]->endpointTop[1] = 5;
	colony.children[4]->endpointBottom[1] = 0;

	// 5
	colony.children[5]->endpointTop[1] = 6;
	colony.children[5]->endpointBottom[1] = 4;
	colony.children[5]->endpointRight[1] = 9;

	// 6
	colony.children[6]->endpointBottom[1] = 5;
	colony.children[6]->endpointLeft[1] = 7;

	// 7
	// FIXME  colony.children[7]->endpointTop[1] = 15;
	colony.children[7]->endpointRight[1] = 6;
	}

	// --- CHAMBERS ---
	// 8
	colony.children[8]->endpointRight[1] = 0;

	// 9
	colony.children[9]->endpointLeft[1] = 5;

	// 10
	colony.children[10]->endpointTop[1] = 2;

	// 11
	colony.children[11]->endpointRight[1] = 12;
	colony.children[11]->endpointLeft[1] = 0;
	colony.children[11]->endpointForward[1] = 13;

	// 12
	colony.children[12]->endpointLeft[1] = 11;
	colony.children[12]->endpointForward[1] = 14;

	// 13
	colony.children[13]->endpointRight[1] = 14;
	colony.children[13]->endpointLeft[1] = 1;
	colony.children[13]->endpointForward[1] = 3;
	colony.children[13]->endpointBack[1] = 11;

	// 14
	colony.children[14]->endpointLeft[1] = 13;
	colony.children[14]->endpointBack[1] = 12;

	// colony.AddHill(-1, 4, 0); // 15
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
