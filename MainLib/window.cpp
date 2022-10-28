#include "window.hpp"

void CreateWindow()
{
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	win = glutCreateWindow("Brian Glassman - Graduate Final Project");

    ErrCheck("CreateWindow");
}