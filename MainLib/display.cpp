
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"

static float view_rotx, view_roty, view_rotz;

void HandleMousePosition()
{
	// Create a deadzone in the middle
	if (mouse_x > -0.1 && mouse_x < 0.1) mouse_x = 0;
	if (mouse_y > -0.1 && mouse_y < 0.1) mouse_y = 0;
	
	/*
	if (viewMode == VIEW_FIRST)
	{
		fp_rotH += 10 * mouse_x * fabs(mouse_x);
		fp_rotV += 10 * mouse_y * fabs(mouse_y);
		
		// Wrap-around horizontal, but clamp vertical
		fp_rotH = fmodf(fp_rotH, 360.0);
		if (fp_rotV > 89.9) fp_rotV = 89.9;
		if (fp_rotV < -89.9) fp_rotV = -89.9;
		
		fp_centX = fp_eyeX + Sin(fp_rotH)*Cos(fp_rotV);
		fp_centY = fp_eyeY - Sin(fp_rotV);
		fp_centZ = fp_eyeZ - Cos(fp_rotH)*Cos(fp_rotV);
	}
	else
	*/
	{
	
		view_roty += 10 * mouse_x * fabs(mouse_x);
		view_rotx += 10 * mouse_y * fabs(mouse_y);
		
		// Wrap-around
		view_roty = fmodf(view_roty, 360.0);
		view_rotx = fmodf(view_rotx, 360.0);
	}
}

void RotateView()
{
	// Assume orthographic FIXME
	glRotated(view_rotx, X_AXIS);
	glRotated(view_roty, Y_AXIS);
	glRotated(view_rotz, Z_AXIS);
}

void preDisplay()
{
	// Clear previous
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	HandleMousePosition();
	RotateView();
}

void postDisplay(float scale)
{
	DrawAxes(scale);

	glFlush();
	glutSwapBuffers();

	ErrCheck("display");
}
void postDisplay()
{
	postDisplay(2);
}

void idle()
{
	glutPostRedisplay();
}

void visible(int vis)
{
	if (vis == GLUT_VISIBLE)
		glutIdleFunc(idle);
	else
		glutIdleFunc(NULL);
}

bool usingWire = false;
void ToggleWireframe()
{
	SetWireframe(!usingWire);
}
void SetWireframe(bool useWire)
{
	if (useWire)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}