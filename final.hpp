#pragma once

#include "CSCIx229.h"

// Window stuff
int windowWidth = 900;
int windowHeight = 900;
static GLint win = 0;

//-----------------------
// Function declarations
//-----------------------
// Display
static void display();
static void idle();
static void visible(int vis);
// Init stuff
static void init(int argc, char* argv[]);
void SetCallbacks();
// Interface