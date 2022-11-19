#include <iostream>

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

int main(int argc, char* argv[])
{
    init(argc, argv);

    SetCallbacks();
	
    display();

    // http://www.cs.sjsu.edu/~bruce/fall_2016_cs_116a_lecture_creating_mouse_driven_menus.html
    // http://www.cs.sjsu.edu/~bruce/programs/fall_2016_cs_116a/example_glut_menu/example_glut_menu.c
    int glut_sub_sub_menu = glutCreateMenu(callback);
    glutAddMenuEntry("selection 1", 1);
    glutAddMenuEntry("selection 2", 2);
    glutAddMenuEntry("selection 3", 3);
    int glut_sub_menu = glutCreateMenu(callback);
    glutAddSubMenu("sub menu", glut_sub_sub_menu);
    glutCreateMenu(callback);
    glutAddSubMenu("menu", glut_sub_menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}