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

int main(int argc, char* argv[])
{
    init(argc, argv);

    SetCallbacks();

	display();

	ErrCheck("main");
	glutMainLoop();

	fprintf(stdout, "Passed loop\n");
	return 99;
}