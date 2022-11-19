#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include "CSCIx229.h"
#include "util.hpp"
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "window.hpp"

//-----------------------
// Function declarations
//-----------------------
static void init(int argc, char* argv[]);
void display();
void SetCallbacks()
{
	glutDisplayFunc(display);
	//FIXME
	glutReshapeFunc(reshape);
	glutVisibilityFunc(visible);

	SetInputCallbacks();

	ErrCheck("SetCallbacks");
}

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

	ErrCheck("init");
}

void display()
{
	preDisplay();

	glDisable(GL_LIGHTING); // FIXME for some reason have to do this every loop?

    static Cube cube;
    cube.Draw();

	postDisplay(2);
}

void callback(int val)
{
    fprintf(stdout, "%d\n", val);
}

class Menu
{
public:
    Menu()
    {
        options = { "Option 0", "Option 1", "Option 2" };
        callbacks = { NULL, NULL, NULL };

        printf("base cons %lu\n", options.size());

        Create();
    }
    int id;
protected:
    virtual void Create()
    {
        // Create the parent menu
        id = glutCreateMenu(callback);

        printf("%lu\n", options.size());

        // Create sub-menus
        for (unsigned int i = 0; i < options.size(); i++)
        {
            fprintf(stdout, "Adding menu\n");
            glutAddMenuEntry(options[i], i);
            if (callbacks[i] != NULL)
            {
                // FIXME
            }
        }
    }
    std::vector<const char*> options;
    std::vector<std::function<void(int)>> callbacks; // https://en.cppreference.com/w/cpp/utility/functional/function
};

class idkMenu : public Menu
{
public:
    idkMenu()
    {
        options = { "Toggle Wireframe" };
        callbacks = { NULL };

        printf("idk cons %lu\n", options.size());

        Create();
    }
};

int main(int argc, char* argv[])
{
    init(argc, argv);

    SetCallbacks();
	
    display();

    // http://www.cs.sjsu.edu/~bruce/fall_2016_cs_116a_lecture_creating_mouse_driven_menus.html
    // http://www.cs.sjsu.edu/~bruce/programs/fall_2016_cs_116a/example_glut_menu/example_glut_menu.c
    idkMenu subSubMenu;
    int glut_sub_menu = glutCreateMenu(callback);
    glutAddSubMenu("sub menu", subSubMenu.id);
    glutCreateMenu(callback);
    glutAddSubMenu("menu", glut_sub_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}