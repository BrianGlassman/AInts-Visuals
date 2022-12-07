#include "Menus.hpp"
#include "CSCIx229.h"
#include "display.hpp"
#include "lighting.hpp"
#include "globals.hpp"
#include "window.hpp"

using namespace Menus;

namespace Callbacks{

void nullopCallback(int val) {};

void printCallback(int val)
{
    printf("Option %d\n", val);
}
}

//-------------
// Menus Class
//-------------
void Menus::CreateMenus()
{
    // Main
    MainMenu mainMenu;
    mainMenu.Create();
    mainMenu.Attach(GLUT_RIGHT_BUTTON);

    // Build
    BuildMenu build; build.Create();
    mainMenu.AddSubMenu(&build);

    // View
    ViewMenu view; view.Create();
    mainMenu.AddSubMenu(&view);

    // Lighting
    LightMenu light; light.Create();
    mainMenu.AddSubMenu(&light);

    // Noise
    NoiseMenu noise; noise.Create();
    mainMenu.AddSubMenu(&noise);

    // Gizmos
    GizmosMenu gizmos; gizmos.Create();
    mainMenu.AddSubMenu(&gizmos);
}

Menu::Menu() { name = "UNSET"; callback = Callbacks::printCallback; }

void Menu::Create()
{
    // Create the parent menu, using the given callback
    id = glutCreateMenu(callback);

    // Create sub-menus
    for (unsigned int i = 0; i < buttons.size(); i++)
    {
        glutAddMenuEntry(buttons[i], i);
    }
}

void Menu::Attach(int button)
{
    glutSetMenu(id);
    glutAttachMenu(button);
}

void Menu::AddSubMenu(Menu* menu)
{
    glutSetMenu(id);
    glutAddSubMenu(menu->name, menu->id);
}

//------
// Main
//------
MainMenu::MainMenu()
{
    name = "Main";
}

//-------
// Build
//-------
namespace Callbacks{ void Build(int val)
{
    if (Globals::viewMode == ViewMode::INTERIOR) return;
    
    switch(val)
    {
    case 0:
        SetBuildModel(StructureType::NONE);
        break;
    case 1:
        SetBuildModel(StructureType::Tunnel);
        break;
    case 2:
        SetBuildModel(StructureType::Chamber);
        break;
    case 3:
        SetBuildModel(StructureType::Mine);
        break;
    case 4:
        SetBuildModel(StructureType::Hill);
        break;
    case 5:
        SetBuildModel(StructureType::Delete);
        break;
    default:
        Fatal(999, "Unknown val %d to Build callback\n", val);
    }
}}
BuildMenu::BuildMenu()
{
    name = "Build";
    buttons = { "None", "Tunnel", "Chamber", "Mine", "Hill", "Delete" };
    callback = Callbacks::Build;
}

//------
// View
//------
namespace Callbacks{ void View(int val)
{
    switch(val)
    {
    case 0:
        Globals::viewMode = ViewMode::EXTERIOR;
        glCullFace(GL_BACK);
        Project();
        break;
    case 1:
        Globals::viewMode = ViewMode::INTERIOR;
        glCullFace(GL_FRONT);
        Project();
        break;
    default:
        Fatal(999, "Unknown val %d to View callback\n", val);
    }
}}
ViewMenu::ViewMenu()
{
    name = "View";
    buttons = { "Exterior", "Interior" };
    callback = Callbacks::View;
}

//--------
// Gizmos
//--------
namespace Callbacks{ void Gizmos(int val)
{
    switch(val)
    {
    case 0:
        Toggles::Display::wireframe = !Toggles::Display::wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, (Toggles::Display::wireframe) ? GL_LINE : GL_FILL);
        break;
    case 1:
        Toggles::Display::showAxes = !Toggles::Display::showAxes;
        break;
    case 2:
        Toggles::showNormals = !Toggles::showNormals;
        break;
    case 3:
        Toggles::showCenterlines = !Toggles::showCenterlines;
        break;
    case 4:
        Toggles::printMove = !Toggles::printMove;
        break;
    case 5:
        Toggles::Display::faceCulling = !Toggles::Display::faceCulling;
        if (Toggles::Display::faceCulling) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
        break;
    case 6:
        CycleSetColor();
        break;
    default:
        Fatal(999, "Unknown val %d to Gizmos callback\n", val);
    }
}}
GizmosMenu::GizmosMenu()
{
    name = "Gizmos";
    buttons = { "Toggle Wireframe", "Toggle Axes", "Toggle Normals",
                "Toggle Centerlines", "Toggle Movement Printout", "Toggle Face Culling",
                "Cycle SetColor Mode" };
    callback = Callbacks::Gizmos;
};

//-------
// Light
//-------
namespace Callbacks{ void Light(int val)
{
    switch(val)
    {
    case 0:
        Globals::Lighting::ambient->Toggle();
        break;
    case 1:
        Globals::Lighting::directed->Toggle();
        break;
    case 2:
        Toggles::Light::lightOrbiting = !Toggles::Light::lightOrbiting;
        break;
    default:
        Fatal(999, "Unknown val %d to Light callback\n");
    }
}}
LightMenu::LightMenu()
{
    name = "Lighting";
    buttons = { "Toggle ambient light", "Toggle directed light", "Toggle auto/manual orbit" };
    callback = Callbacks::Light;
};

//-------
// Noise
//-------
namespace Callbacks{ void Noise(int val)
{
    switch(val)
    {
    case 0: // Toggle perturbed geometry
        Toggles::Noise::showPerturbed = !Toggles::Noise::showPerturbed;
        if (! (Toggles::Noise::showPerturbed || Toggles::Noise::showBase))
        { // All geometry turned off, turn base back on to make sure something is shown
            Toggles::Noise::showBase = true;
        }
        break;
    case 1:
        Toggles::Noise::showPVectors = !Toggles::Noise::showPVectors;
        break;
    case 2:
        Toggles::Noise::showBase = !Toggles::Noise::showBase;
        if (! (Toggles::Noise::showPerturbed || Toggles::Noise::showBase))
        { // All geometry turned off, turn noise back on to make sure something is shown
            Toggles::Noise::showPerturbed = true;
        }
        break;
    case 3:
        Toggles::Noise::bezierFade = !Toggles::Noise::bezierFade;
        displayModelPtr->ApplyNoise();
        break;
    default:
        Fatal(999, "Unknown val %d to Noise callback\n");
    }
}}
NoiseMenu::NoiseMenu()
{
    name = "Noise";
    buttons = { "Toggle perturbed geometry", "Toggle P Vectors", "Toggle base geometry", "Toggle Bezier fade" };
    callback = Callbacks::Noise;
}