#pragma once

#include "CSCIx229.h"
#include "display.hpp"
#include "input.hpp"
#include "window.hpp"

//-----------------------
// Function declarations
//-----------------------
// Init stuff
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