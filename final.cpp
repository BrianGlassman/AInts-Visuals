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
	LoadTexBMP("Textures/", "dirtGroundTileable.bmp");
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

	// Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, (Toggles::Display::wireframe) ? GL_LINE : GL_FILL);

	SetCallbacks();

	Menus::CreateMenus();

	InitShaders();

	InitObjects();

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
	case Scene::mine:
	case Scene::farm:
	case Scene::hill:
	{
		baseMag = 2;
		break;
	}
	case Scene::allTunnels:
	{
		baseMag = 4;
		break;
	}
	default:
		Fatal(999, "final.cpp::display: Unknown scene %d\n", Globals::sceneChoice);
	}

	orbiterPtr->UpdatePosition(); // FIXME this should probably not be in display
	orbiterPtr->Draw();

	// Use brick shader as a scream test
	UseShader(Shader::brickShader);
	
	if (displayModelPtr->created)
	{
		// Make sure that eyePos updates with any scene or geometry changes
		auto& CL = *(currentStructure->getCL());
		Globals::InteriorView::eyePos = CL[currentCLidx].coords;

		displayModelPtr->Draw();
	}

	//  Revert to fixed pipeline
	UseShader(Shader::fixedPipeline);




	// FIXME this should very much be elsewhere
	glCallList(Objects::Mine);






	if (Globals::toBuild != StructureType::NONE) buildIndicator.Draw();

	if (Toggles::Noise::showPVectors) noisePtr->DrawNoise();

	// In exterior view, draw an indicator of where the interior view is
	if (Globals::viewMode == ViewMode::EXTERIOR && displayModelPtr->created)
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

// Connect Input to the BuildIndicator
void Input::buildKey(unsigned char k)
{
	if (k == 32) // Space - build it
	{
		switch(Globals::toBuild)
		{
		case StructureType::NONE: return;
		case StructureType::Delete:
			if (displayModelPtr->GetChild(buildIndicator.center) == nullptr)
			{
				// Nothing there, nothing to do
				return;
			}
			displayModelPtr->DeleteStructure(buildIndicator.center);
			
			buildIndicator.Create();
			buildIndicator.ApplyNoise();
			break;
		default: displayModelPtr->AddStructure(buildIndicator.center, Globals::toBuild); break;
		}
		
		displayModelPtr->Create();
		displayModelPtr->ApplyNoise();
	}
	else
	{
		buildIndicator.HandleKey(k);
	}
}

// Connect the Menu to the BuildIndicator
void Menus::SetBuildModel(StructureType type)
{
	buildIndicator.SetModel(type);
}

int main(int argc, char* argv[])
{
	Perlin noise;
	noisePtr = &noise;

	{
		bool startNoisy = true;
		Toggles::Noise::showPerturbed = startNoisy;
		Toggles::Noise::showBase = !startNoisy;
	}

	// Create the scene to be displayed
	Globals::sceneChoice = Scene::colony;

	if (true) // Colony
	{
		PopulateColony(colony);
		colony.Create();
		colony.ApplyNoise();
	}

	if (true) // Tunnel
	{
		tunnel.AddTunnel(0, 0, 0);
		tunnel.AddTunnel(-1, 0, 0);
		tunnel.Create();
		tunnel.ApplyNoise();
	}

	if (true) // Chamber
	{
		chamber.AddChamber(0, 0, 0);
		chamber.Create();
		chamber.ApplyNoise();
	}

	if (true) // Mine
	{
		// TODO
	}

	if (true) // Farm
	{
		// TODO
	}

	if (true) // Hill
	{
		hill.AddHill(0, 0, 0);
		hill.Create();
		hill.ApplyNoise();
	}

	if (true) // All Tunnels
	{
		PopulateTunnels(allTunnels);
		allTunnels.Create();
		allTunnels.ApplyNoise();
	}

	// Initialize displayModelPtr
	SetDisplayPtr();

	Menus::SetBuildModel(StructureType::NONE);

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