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
void PushShader(int shader); // Save the current shader to the stack and start using the chosen shader
void PopShader(); // Start using the top-most shader from the stack

int CreateShaderProg(std::string VertFile, std::string FragFile);