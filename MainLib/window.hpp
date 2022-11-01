#pragma once

#include "CSCIx229.h"

extern int windowWidth;
extern int windowHeight;
extern GLint win;

void CreateWindow();
void Cleanup();
void reshape(int width,int height);
void Project();