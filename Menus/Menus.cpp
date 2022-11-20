#include "CSCIx229.h"
#include "display.hpp"
#include "Menus.hpp"
#include "globals.hpp"

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

    // idk
    idkMenu idk; idk.Create();
    mainMenu.AddSubMenu(&idk);
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
        // FIXME Exterior
        break;
    case 1:
        // FIXME Interior
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

//-----
// IDK
//-----
namespace Callbacks{ void idk(int val)
{
    switch(val)
    {
    case 0:
        ToggleWireframe();
        break;
    default:
        Fatal(999, "Unknown val %d to idk callback\n", val);
    }
}}
idkMenu::idkMenu()
{
    name = "idk";
    buttons = { "Toggle Wireframe" };
    callback = Callbacks::idk;
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
        lightOrbiting = !lightOrbiting;
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