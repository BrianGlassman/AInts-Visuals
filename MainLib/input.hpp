#pragma once

#include "CSCIx229.h"

#include "util.hpp" // FIXME for temp only
#include "Colony.hpp"

namespace Input {
    void buildKey(unsigned char k);
}

void SetInputCallbacks();

// FIXME this is super temp and hacky
extern Colony* currentStructure;
extern int currentCLidx;
void PrintMoveOptions();