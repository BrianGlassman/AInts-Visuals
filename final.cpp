/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "final.hpp"

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
    // for SOME REASON having this load here makes the one inside textures.cpp work
    LoadTexBMP("Textures/", "dirtGroundTileable.bmp");
    InitTextures();
    BindTexture("dirt");

    // Lighting
    initLighting();
    Light ambient(0.5, 0, 0);
    Globals::Lighting::ambient = &ambient;
    // OrbitLight orbiter(0, 0.45, 0.7);
    OrbitLight orbiter(0, 0.4, 0);
    Globals::Lighting::directed = &orbiter;
    orbiter.radius = baseMag * 1.2;

    // Enable alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Face culling and Z-buffering
    if (Toggles::Display::faceCulling) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
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

    Globals::Lighting::directed->UpdatePosition();
    Globals::Lighting::directed->Draw();
    
    if (displayModelPtr->created)
    {
        // Make sure that eyePos updates with any scene or geometry changes
        auto CL = Globals::InteriorView->pathHolder->getCL();
        Globals::InteriorView->eyePos = CL->at(Globals::InteriorView->pathIdx).coords;

        displayModelPtr->Draw();
    }

    if (Globals::toBuild != StructureType::NONE) buildIndicator.Draw();

    if (Toggles::Noise::showPVectors) noisePtr->DrawNoise();

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
    InitOBJs();

    Perlin noise(1670526744);
    noisePtr = &noise;

    {
        bool startNoisy = true;
        Toggles::Noise::showPerturbed = startNoisy;
        Toggles::Noise::showBase = !startNoisy;
    }

    // Create the scene to be displayed
    Globals::sceneChoice = Scene::colony;
    Globals::InteriorView = &(Globals::IVs.at(Globals::sceneChoice));

    if (true) // Colony
    {
        PopulateColony(colony);
        colony.Create();
        colony.ApplyNoise();
        Globals::IVs.at(Scene::colony).pathHolder = &colony;
    }

    if (true) // Tunnel
    {
        tunnel.AddTunnel(0, 0, 0);
        tunnel.AddTunnel(-1, 0, 0);
        tunnel.Create();
        tunnel.ApplyNoise();
        Globals::IVs.at(Scene::tunnel).pathHolder = &tunnel;
    }

    if (true) // Chamber
    {
        chamber.AddChamber(0, 0, 0);
        chamber.Create();
        chamber.ApplyNoise();
        Globals::IVs.at(Scene::chamber).pathHolder = &chamber;
    }

    if (true) // Mine
    {
        mine.AddStructure(0, 0, 0, StructureType::Mine);
        mine.Create();
        mine.ApplyNoise();
        Globals::IVs.at(Scene::mine).pathHolder = &mine;
    }

    if (true) // Hill
    {
        hill.AddHill(0, 0, 0);
        hill.Create();
        hill.ApplyNoise();
        Globals::IVs.at(Scene::hill).pathHolder = &hill;
    }

    if (true) // All Tunnels
    {
        PopulateTunnels(allTunnels);
        allTunnels.Create();
        allTunnels.ApplyNoise();
        Globals::IVs.at(Scene::allTunnels).pathHolder = &allTunnels;
    }

    // Initialize displayModelPtr
    SetDisplayPtr();

    Menus::SetBuildModel(StructureType::NONE);

    init(argc, argv);

    if (Toggles::printMove) PrintMoveOptions();

    ErrCheck("main");
    glutMainLoop();

    fprintf(stdout, "Passed loop\n");
    return 99;
}