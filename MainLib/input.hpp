#pragma once

#include "CSCIx229.h"

#include "Structure.hpp" // FIXME for temp only

void SetInputCallbacks();

// FIXME this is super temp and hacky
extern Structure* currentStructure;
extern int currentCLidx;
void PrintMoveOptions();