/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "final.hpp"
#include "display.hpp"
#include "input.hpp"
#include "window.hpp"

static void init(int argc, char* argv[])
{
	// Do all the OpenGL setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	
    CreateWindow();
	
	#ifdef USEGLEW
		if (glewInit() != GLEW_OK)
		{
			Fatal(1, "Error initializing GLEW\n");
		}
	#endif

    //FIXME
	//initLighting();
	//initTextures();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	ErrCheck("init");
}

void SetCallbacks()
{
	glutDisplayFunc(display);
    //FIXME
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	//glutSpecialFunc(special);
	glutPassiveMotionFunc(mouse);
	glutVisibilityFunc(visible);

    ErrCheck("SetCallbacks");
}

int main(int argc, char* argv[])
{
	init(argc, argv);

    SetCallbacks();

	// Run display and reshape to zoom to fit
	display();
	//reshape(windowWidth, windowHeight; FIXME

	ErrCheck("main");
	glutMainLoop();
	
	fprintf(stdout, "Passed loop\n");
	return 99;
}