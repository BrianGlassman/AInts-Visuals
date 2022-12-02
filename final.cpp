/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "final.hpp"

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

void display()
{
	preDisplay();

	// Change scenes if needed
	SetDisplayPtr();
	switch(Globals::sceneChoice)
	{
	case Scene::colony:
	{
		baseMag = 4;
		break;
	}
	case Scene::tunnel:
	{
		baseMag = 1;
		break;
	}
	case Scene::chamber:
	case Scene::hill:
	{
		baseMag = 2;
		break;
	}
	// case Scene::allTunnels:
	// {
	// 	baseMag = 4;
	// 	break;
	// }
	default:
		Fatal(999, "Unknown scene %d\n", Globals::sceneChoice);
	}

	// Make sure that eyePos updates with any scene or geometry changes
	auto& CL = *(currentStructure->getCL());
	Globals::InteriorView::eyePos = CL[currentCLidx].coords;

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
	Perlin noise;
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
		tunnel.AddTunnel(-1, 0, 0, 16);
		if (useNoise) tunnel.ApplyNoise(zero);
		tunnel.Create();
	}

	if (true)
	{
		chamber.AddChamber(0, 0, 0, 4); // Chamber
		chamber.AddChamber(0, 1, 0, 8);
		if (useNoise) chamber.ApplyNoise(zero);
		chamber.Create();
	}

	if (true)
	{
		hill.AddHill(0, 0, 0);
		hill.AddTunnel(0, -1, 0, 4);
		if (useNoise) hill.ApplyNoise(zero);
		hill.Create();
	}

	// if (false) // FIXME CL breaks because the connections are wrong (i.e. forced)
	// {
	// 	PopulateTunnels(allTunnels);
	// 	if (useNoise) allTunnels.ApplyNoise(zero);
	// 	allTunnels.Create();
	// }

	// Initialize displayModelPtr
	SetDisplayPtr();

	init(argc, argv);

	currentStructure = displayModelPtr;
	currentCLidx = 0;

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	// PrintMoveOptions();

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}