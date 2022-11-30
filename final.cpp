/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

// MainLib
#include "final.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "lighting.hpp"
#include "textures.hpp"

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
Colony allTunnels;

Noise* noisePtr;

OrbitLight* orbiterPtr;

// Tracks which vertex the interior view is at
Colony* currentStructure;
int currentCLidx;

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
	BindTexture("dirt"); // FIXME each thing should choose the right texture, not rely on this default

	// Lighting
	initLighting();
	Light ambient(0.5, 0, 0);
	// OrbitLight orbiter(0, 0.45, 0.7);
	OrbitLight orbiter(0, 0.4, 0);
	orbiter.radius = baseMag * 1.2;
	orbiterPtr = &orbiter;

	// Enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// Face culling and Z-buffering
	glEnable(GL_CULL_FACE);
	if (Globals::viewMode == ViewMode::EXTERIOR)
		glCullFace(GL_BACK);
	else
		glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	SetCallbacks();

	Menus::CreateMenus();

	InitShaders();

	ErrCheck("init");
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

void display()
{
	preDisplay();

	// Change scenes if needed
	switch(Globals::sceneChoice)
	{
	case Scene::colony:
	{
		baseMag = 4;
		displayModelPtr = &colony;
		break;
	}
	case Scene::tunnel:
	{
		baseMag = 1;
		displayModelPtr = &tunnel;
		break;
	}
	case Scene::chamber:
	{
		baseMag = 2;
		displayModelPtr = &chamber;
		break;
	}
	case Scene::allTunnels:
	{
		baseMag = 4;
		displayModelPtr = &allTunnels;
		break;
	}
	default:
		Fatal(999, "Unknown scene %d\n", Globals::sceneChoice);
	}

	orbiterPtr->UpdatePosition(); // FIXME this should probably not be in display
	orbiterPtr->Draw();

	if (true)
	{
		UseShader(Shader::threeDshader);
	}

	displayModelPtr->Draw();

	//  Revert to fixed pipeline
	UseShader(Shader::fixedPipeline);

	if (Toggles::Noise::showPVectors) noisePtr->DrawNoise();

	// In exterior view, draw an indicator of where the interior view is
	if (Globals::viewMode == ViewMode::EXTERIOR)
	{
		PushShader(0);
		glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glPointSize(20);
		glColor3f(1, 0, 0);

		auto& eyePos = Globals::InteriorView::eyePos;
		glBegin(GL_POINTS); glVertex3f(eyePos.x, eyePos.y, eyePos.z); glEnd();

		glColor3f(1, 1, 1);
		glPopAttrib();
		PopShader();
	}

	ErrCheck("display");

	postDisplay();
}

int main(int argc, char* argv[])
{
	Perlin noise(1669792348);
	noisePtr = &noise;

	bool useNoise = true;
	Toggles::Noise::showPerturbed = useNoise;
	Toggles::Noise::showBase = !useNoise;
	float zero[] = {0, 0, 0}; // FIXME not sure why the 1-param ApplyNoise isn't getting inherited

	// Create the scene to be displayed
	Globals::sceneChoice = Scene::colony;

	if (true)
	{
		PopulateColony(colony);
		if (useNoise) colony.ApplyNoise(zero);
		colony.Create();
	}

	// forward : 1
	//    back : 2
	//     top : 4
	//  bottom : 8
	//   right : 16
	//    left : 32
	if (true)
	{
		tunnel.AddTunnel(0, 0, 0, 32); // Tunnel
		tunnel.children[0]->endpointLeft[1] = 1;
		tunnel.AddTunnel(-1, 0, 0, 16);
		tunnel.children[1]->endpointRight[1] = 0;
		if (useNoise) tunnel.ApplyNoise(zero);
		tunnel.Create();
	}

	if (true)
	{
		chamber.AddChamber(0, 0, 0, 4); // Chamber
		chamber.children[0]->endpointTop[1] = 1;
		chamber.AddChamber(0, 1, 0, 8);
		chamber.children[1]->endpointBottom[1] = 0;
		if (useNoise) chamber.ApplyNoise(zero);
		chamber.Create();
	}

	if (false) // FIXME CL breaks because the connections are wrong (i.e. forced)
	{
		PopulateTunnels(allTunnels);
		if (useNoise) allTunnels.ApplyNoise(zero);
		allTunnels.Create();
	}

	// Initialize displayModelPtr
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
	case Scene::allTunnels:
	{
		displayModelPtr = &allTunnels;
		break;
	}
	default:
		Fatal(999, "Unknown scene %d\n", Globals::sceneChoice);
	}

	init(argc, argv);

	currentStructure = displayModelPtr;
	currentCLidx = 0;
	auto& CL = *(currentStructure->getCL());
	Globals::InteriorView::eyePos = CL[currentCLidx].coords;

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	// PrintMoveOptions();

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}