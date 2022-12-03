#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class and DrawAxes
#include "Colony.hpp"

extern Colony* displayModelPtr;

void preDisplay();
void postDisplay(float scale);
void postDisplay();
void idle();
void visible(int vis);