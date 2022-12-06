
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "Shaders.hpp"

static float view_rotx, view_roty, view_rotz;

void HandleMousePosition()
{
	if (Globals::viewMode == ViewMode::INTERIOR)
	{
		auto& iv = Globals::InteriorView;

		iv->rotH += 10 * Globals::mouse_x * fabs(Globals::mouse_x);
		iv->rotV += 10 * Globals::mouse_y * fabs(Globals::mouse_y);
		
		// Wrap-around horizontal
		iv->rotH = fmodf(iv->rotH, 360.0);
		if (iv->rotH < 0) iv->rotH = 360 + iv->rotH;
		// printf("rotH = %f\n", iv->rotH);

		// Clamp vertical
		if (iv->rotV > 89.9) iv->rotV = 89.9;
		if (iv->rotV < -89.9) iv->rotV = -89.9;
		// printf("rotV = %f\n", iv->rotV);

		// Update view vector
		iv->lookDir[0] =  Sin(iv->rotH)*Cos(iv->rotV);
		iv->lookDir[1] = -Sin(iv->rotV);
		iv->lookDir[2] = -Cos(iv->rotH)*Cos(iv->rotV);
		// printf("lookDir (%f, %f, %f)\n", iv->lookDir[0], iv->lookDir[1], iv->lookDir[2]);

		// Update Right to match new view vector
		iv->right = iv->lookDir.Cross(iv->up);
		iv->right.Normalize();
	}
	else
	{
		view_roty += 10 * Globals::mouse_x * fabs(Globals::mouse_x);
		view_rotx += 10 * Globals::mouse_y * fabs(Globals::mouse_y);
		
		// Wrap-around horizontal
		view_roty = fmodf(view_roty, 360.0);

		// Clamp vertical
		if (view_rotx > 89.9) view_rotx = 89.9;
		if (view_rotx < -89.9) view_rotx = -89.9;
		// printf("view_rotx = %f\n", view_rotx);
	}
}

void RotateView()
{
	// Note: only works for orthographic
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

	switch (Globals::viewMode)
	{
	case ViewMode::EXTERIOR:
		RotateView();
		break;
	case ViewMode::INTERIOR:
	{
		auto& iv = Globals::InteriorView;
		gluLookAt(
			iv->eyePos[0],
			iv->eyePos[1],
			iv->eyePos[2],
			iv->eyePos[0] + iv->lookDir[0],
			iv->eyePos[1] + iv->lookDir[1],
			iv->eyePos[2] + iv->lookDir[2],
			iv->up[0],
			iv->up[1],
			iv->up[2]);
		break;
	}
	default:
		FatalDef();
	}

	// Use brick shader as a scream test
	UseShader(Shader::brickShader);

	ErrCheck("PreDisplay");
}

void postDisplay(float scale)
{
	// In exterior view, draw an indicator of where the interior view is
	if (Globals::viewMode == ViewMode::EXTERIOR && displayModelPtr->created)
	{
		PushShader(Shader::fixedPipeline);
		glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glPointSize(20);
		glColor3f(1, 0, 0);

		auto& eyePos = Globals::InteriorView->eyePos;
		glBegin(GL_POINTS); glVertex3f(eyePos.x, eyePos.y, eyePos.z); glEnd();

		glColor3f(1, 1, 1);
		glPopAttrib();
		PopShader();
	}

	if (Toggles::Display::showAxes) DrawAxes(scale);

	glFlush();
	glutSwapBuffers();

	ErrCheck("postDisplay");
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