
#include "util.h"
#include "display.hpp"
#include "input.hpp"

static float view_rotx, view_roty, view_rotz;

/*
 * Draws a triangle and sets the normal vector
*/
void DrawLitQuad(const float A[], const float B[], const float C[], const float D[])
{
	// Based on triangle function in Ex. 13 via DrawLitTriangle
	
	// Planar vector 0
	float dx0 = A[0]-B[0];
	float dy0 = A[1]-B[1];
	float dz0 = A[2]-B[2];
	// Planar vector 1
	float dx1 = C[0]-A[0];
	float dy1 = C[1]-A[1];
	float dz1 = C[2]-A[2];
	// Normal
	float Nx = dy0*dz1 - dy1*dz0;
	float Ny = dz0*dx1 - dz1*dx0;
	float Nz = dx0*dy1 - dx1*dy0;
	// Draw quad
	glNormal3f(-Nx, -Ny, -Nz);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(A[0],A[1],A[2]);
	glTexCoord2f(1, 0);
	glVertex3f(B[0],B[1],B[2]);
	glTexCoord2f(1, 1);
	glVertex3f(C[0],C[1],C[2]);
	glTexCoord2f(0, 1);
	glVertex3f(D[0],D[1],D[2]);
	glEnd();
}

/*
 * Draws a cube
 * sides - binary flags {X = 0 (32), X = 1 (16), Y = 0 (8), Y = 1 (4), Z = 0 (2), Z = 1 (1)}
*/
void DrawCube(const float x, const float y, const float z,
	const float xs, const float ys, const float zs,
	const unsigned char sides)
{
	// Vertex Coordinates
	const float vertices[][3] = {
		{0, 0, 0}, // 0 - left bottom back
		{1, 0, 0}, // 1 - right bottom back
		{1, 1, 0}, // 2 - right top back
		{1, 0, 1}, // 3 - right bottom front
		{1, 1, 1}, // 4 - right top front
		{0, 1, 0}, // 5 - left top back
		{0, 1, 1}, // 6 - left top front
		{0, 0, 1}, // 7 - left bottom front
	};
	
	// Indices
	const unsigned char indices[][4] = {
		{7, 6, 5, 0}, // X = 0
		{1, 2, 4, 3}, // X = 1
		{0, 1, 3, 7}, // Y = 0
		{5, 6, 4, 2}, // Y = 1
		{5, 2, 1, 0}, // Z = 0
		{3, 4, 6, 7}, // Z = 1
	};
	
	glPushMatrix(); {
		// Apply transformations
		glTranslatef(x, y, z); // Translate to chosen center
		glScalef(xs, ys, zs); // Apply scaling
		glTranslatef(-0.5, -0.5, -0.5); // Center at target coords
		
		for (int i=0; i<6; i++)
		{
			if (sides & (1 << i))
			{
				//fprintf(stdout, "%d & %d\n", sides, i);
				DrawLitQuad(
					vertices[indices[5-i][0]],
					vertices[indices[5-i][1]],
					vertices[indices[5-i][2]],
					vertices[indices[5-i][3]]
				);
			}
		}
	} glPopMatrix();
}

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

void DrawScene()
{
	// TODO get rid of this
	DrawCube(0, 0, 0, 1, 1, 1, 63);
}

void display()
{
	// Clear previous
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	HandleMousePosition();
	RotateView();

	// Drawing the scene must be AFTER rotation
	DrawScene();

	glFlush();
	glutSwapBuffers();

	ErrCheck("display");
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