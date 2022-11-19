#include "CSCIx229.h"
#include "Menus.hpp"

void callback(int val)
{
    fprintf(stdout, "%d\n", val);
}

Menu::Menu()
{
    options = { "Option 0", "Option 1", "Option 2" };
    callbacks = { NULL, NULL, NULL };
}

void Menu::Create()
{
    // Create the parent menu
    id = glutCreateMenu(callback);

    // Create sub-menus
    for (unsigned int i = 0; i < options.size(); i++)
    {
        glutAddMenuEntry(options[i], i);
        if (callbacks[i] != NULL)
        {
            // FIXME
        }
    }
};

idkMenu::idkMenu()
{
    options = { "Toggle Wireframe" };
    callbacks = { NULL };
};

LightMenu::LightMenu()
{
    options = { "Toggle ambient light", "Toggle directed light" };
    callbacks = { NULL, NULL, NULL };
};