#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class and DrawAxes

extern Model* displayModelPtr;

void display();
void idle();
void visible(int vis);