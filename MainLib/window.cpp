#include "window.hpp"
#include "globals.hpp"
#include "input.hpp"

static GLint win = 0;

void CreateWindow()
{
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	win = glutCreateWindow("Brian Glassman - Graduate Final Project");

	ErrCheck("CreateWindow");
}

void Cleanup(void)
{
	glutDestroyWindow(win);
}

void Project()
{
	int viewMode;
	/* Reference: In-class Ex 9
	 * Adapted to my version of mode-setting and window sizing
	 */
	//  Tell OpenGL we want to manipulate the projection matrix
	glMatrixMode(GL_PROJECTION);
	//  Undo previous transformations
	glLoadIdentity();
	switch (viewMode)
	{
	/*
	case VIEW_PERSP:
		gluPerspective(60, _width / _height, maxMag*0.4, maxMag*10);
		break;
	case VIEW_FIRST:
		gluPerspective(60, _width / _height, maxMag*0.1, maxMag*10);
		break;
	case VIEW_ORTHO:
	*/
	default: // default to orthographic
		//  Orthogonal projection box adjusted for the aspect ratio of the window
		{
			// TODO scale based on the smaller of width/height rather than always using height
			double asp = (windowHeight>0) ? (double)windowWidth/windowHeight : 1;
			float maxMag = baseMag * Globals::mouse_zoom; // FIXME
			// glOrtho(left, right, bottom, top, near, far)
			glOrtho(-asp*maxMag, +asp*maxMag, -maxMag, +maxMag, -maxMag*1.5, +maxMag*1.5);
		}
		break;
	}
	//  Switch to manipulating the model matrix
	glMatrixMode(GL_MODELVIEW);
	//  Undo previous transformations
	glLoadIdentity();
	
	ErrCheck("Project");
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
	windowWidth = width;
	windowHeight = height;
	//  Set the viewport to the entire window
	glViewport(0,0, RES*width,RES*height);

	Project();

	ErrCheck("reshape");
}