#pragma once

#include <stdio.h>
#include <string>

#include "CSCIx229.h"

namespace Objects {
    int Mine;
    int Farm;
}

void InitObjects()
{
    Objects::Mine = LoadOBJ("Objects/", "Mine.obj");
}