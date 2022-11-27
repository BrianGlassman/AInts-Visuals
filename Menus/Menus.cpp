#include "CSCIx229.h"
#include "display.hpp"
#include "Menus.hpp"
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
        Toggles::debug = !Toggles::debug;
        break;
    case 1:
        ToggleWireframe();
        break;
    case 2:
        ToggleAxes();
        break;
    case 3:
        Toggles::showNormals = !Toggles::showNormals;
        break;
    case 4:
        Toggles::showCenterlines = !Toggles::showCenterlines;
        break;
    default:
        Fatal(999, "Unknown val %d to Gizmos callback\n", val);
    }
}}
GizmosMenu::GizmosMenu()
{
    name = "Gizmos";
    buttons = { "Toggle Debug", "Toggle Wireframe", "Toggle Axes", "Toggle Normals",
                "Toggle Centerlines"};
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
        // FIXME toggle ambient
        break;
    case 1:
        // FIXME toggle direct
        break;
    case 2:
        Toggles::Light::lightOrbiting = !Toggles::Light::lightOrbiting;
        break;
    case 3:
        Toggles::Light::smooth = !Toggles::Light::smooth;
        glShadeModel(Toggles::Light::smooth ? GL_SMOOTH : GL_FLAT);
        break;
    default:
        Fatal(999, "Unknown val %d to Light callback\n");
    }
}}
LightMenu::LightMenu()
{
    name = "Lighting";
    buttons = { "Toggle ambient light", "Toggle directed light", "Toggle auto/manual orbit", "Toggle Smooth/Flat" };
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
    default:
        Fatal(999, "Unknown val %d to Noise callback\n");
    }
}}
NoiseMenu::NoiseMenu()
{
    name = "Noise";
    buttons = { "Toggle perturbed geometry", "Toggle P Vectors", "Toggle base geometry" };
    callback = Callbacks::Noise;
}