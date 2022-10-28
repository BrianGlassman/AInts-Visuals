#pragma once

#include "CSCIx229.h"

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