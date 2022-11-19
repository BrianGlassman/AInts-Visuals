#include "CSCIx229.h"
#include "display.hpp"
#include "Menus.hpp"

void nullopCallback(int val) {};

void printCallback(int val)
{
    printf("Option %d\n", val);
}

namespace Callbacks{} ;

Menu::Menu() { name = "UNSET"; callback = printCallback; }

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

MainMenu::MainMenu()
{
    name = "Main";
}

namespace Callbacks{ void idk(int val)
{
    switch(val)
    {
    case 0:
        ToggleWireframe();
        break;
    default:
        Fatal(999, "Unknown val %d\n", val);
    }
} }
idkMenu::idkMenu()
{
    name = "idk";
    buttons = { "Toggle Wireframe" };
    callback = Callbacks::idk;
};

LightMenu::LightMenu()
{
    name = "Lighting";
    buttons = { "Toggle ambient light", "Toggle directed light" };
};