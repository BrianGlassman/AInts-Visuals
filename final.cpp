/* Author: Brian Glassman
 * Date: October 27, 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "final.hpp"
#include "window.hpp"

static void display()
{
	// Clear previous
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glFlush();
	glutSwapBuffers();

	ErrCheck("display");
}

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
	//glutReshapeFunc(reshape);
	//glutKeyboardFunc(key);
	//glutPassiveMotionFunc(mouse); // callback for when mouse is moved in window
	//glutSpecialFunc(special);
	glutVisibilityFunc(visible);

    ErrCheck("SetCallbacks");
}

static void idle()
{
	glutPostRedisplay();
}

static void visible(int vis)
{
	if (vis == GLUT_VISIBLE)
		glutIdleFunc(idle);
	else
		glutIdleFunc(NULL);
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