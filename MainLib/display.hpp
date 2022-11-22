#pragma once

#include "CSCIx229.h"
#include "util.hpp" // Model class and DrawAxes

extern Model* displayModelPtr;

void preDisplay();
void postDisplay(float scale);
void postDisplay();
void idle();
void visible(int vis);

void ToggleWireframe();
void SetWireframe(bool useWire);

void ToggleAxes();
void SetAxes(bool useAxes);