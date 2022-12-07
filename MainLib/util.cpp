#include "util.hpp"
#include "Shaders.hpp"
#include "globals.hpp"

/*
 *  Convert polar to Cartesian coordinates
 */
void Polar2Cart(float radius, float theta, float* x_out, float* y_out)
{
   *x_out = radius * Cos(theta);
   *y_out = radius * Sin(theta);
}

/*
 * Draw the Axes, with a given scale:
 *   X - red
 *   Y - green
 *   Z - blue
 */
void DrawAxes(float scale)
{
	PushShader(Shader::fixedPipeline);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	glPushMatrix(); {
			glScaled(scale, scale, scale);
	
			SetColor(1,0,0);
			glBegin(GL_LINES); {
					glVertex3d(ORIGIN);
					glVertex3d(X_AXIS);
			} glEnd();
			
			SetColor(0,1,0);
			glBegin(GL_LINES); {
					glVertex3d(ORIGIN);
					glVertex3d(Y_AXIS);
			} glEnd();
			
			SetColor(0,0,1);
			glBegin(GL_LINES); {
					glVertex3d(ORIGIN);
					glVertex3d(Z_AXIS);
			} glEnd();
	} glPopMatrix();
	
	glPopAttrib();
	SetColor(1, 1, 1);
	PopShader();
	
	ErrCheck("DrawAxes");
}

/*
 * Draws a quad and sets the normal vector
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

void CycleSetColor()
{
	Globals::setColorMode++;
	switch (Globals::setColorMode)
	{
	case SetColorMode::glColor:
		printf("SetColor Mode set to: glColor\n");
		break;
	case SetColorMode::materialProps:
		printf("SetColor Mode set to: material properties\n");
		break;
	case SetColorMode::both:
		printf("SetColor Mode set to: both\n");
		// TODO
		break;
	default:
		Fatal(999, "Unrecognized SetColorMode");
	}
}
void SetColor(float r, float g, float b, float a)
{
	switch (Globals::setColorMode)
	{
	case SetColorMode::glColor:
		glColor4f(r, g, b, a);
		break;
	case SetColorMode::materialProps:
		glColor4f(b, r, g, a); // TODO (this is just for now)
		break;
	case SetColorMode::both:
		glColor4f(r, g, b, a);
		// TODO
		break;
	default:
		Fatal(999, "Unrecognized SetColorMode");
	}
}