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
#include "BuildIndicator.hpp"

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

BuildIndicator buildIndicator;

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
	buildIndicator.colony = displayModelPtr;
}

void PopulateColony(Colony& colony)
{
	int tunnelCoords[][3] = {
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

	int chamberCoords[][3] = {
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

	Vector3Int coords;
	for (int i = 0; i < 8; i++)
	{
		coords = tunnelCoords[i];
		coords.y -= 1;
		colony.AddTunnel(coords);
	}
	for (int i = 0; i < 7; i++)
	{
		coords = chamberCoords[i];
		coords.y -= 1;
		colony.AddChamber(coords);
	}
	colony.AddHill(-1, 4 - 1, 0);
}

void PopulateTunnels(Colony& colony)
{
	int tunnelCoords[][3] = {
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

	for (int i = 0; i < 27; i++)
	{
		colony.AddTunnel(tunnelCoords[i]);
	}
}
