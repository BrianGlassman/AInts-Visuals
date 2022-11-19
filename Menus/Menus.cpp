#include "CSCIx229.h"
#include "Menus.hpp"

void nullopCallback(int val) {};

void callback(int val)
{
    fprintf(stdout, "%d\n", val);
}

Menu::Menu()
{
}

void Menu::Create()
{
    // Create the parent menu
    id = glutCreateMenu(callback);

    // Create sub-menus
    for (unsigned int i = 0; i < buttons.size(); i++)
    {
        glutAddMenuEntry(buttons[i], i);
    }

    // Assign callbacks
    for (unsigned int i = 0; i < callbacks.size(); i++)
    {
        // FIXME
    }
};

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

idkMenu::idkMenu()
{
    name = "idk";
    buttons = { "Toggle Wireframe" };
};

LightMenu::LightMenu()
{
    name = "Lighting";
    buttons = { "Toggle ambient light", "Toggle directed light" };
};