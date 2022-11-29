#pragma once

#include <stdio.h>
#include <string>

#include "CSCIx229.h"

namespace Shader {
    extern int fixedPipeline;
    extern int brickShader;
    extern int threeDshader;
}

void InitShaders();
void UseShader(int shader);

int CreateShaderProg(std::string VertFile, std::string FragFile);