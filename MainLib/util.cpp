#include "util.hpp"

/*
 *  Convert polar to Cartesian coordinates
 */
void Polar2Cart(float radius, float theta, float* x_out, float* y_out)
{
   *x_out = radius * Cos(theta);
   *y_out = radius * Sin(theta);
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

Cylinder::Cylinder()
{
}

void Cylinder::Draw()
{
    glPushMatrix(); {
        glTranslatef(center[0], center[1], center[2]);
        glScalef(radius, height, radius);
        glTranslatef(0, -0.5, 0);

        // Top
        if (top)
        {
            glBegin(GL_TRIANGLE_FAN); {
                glNormal3f(0, 1, 0);
                glTexCoord2f(0.5, 0.5);
                glVertex3d(0, 1, 0);
                float x, z;
                for (int theta=360; theta >= 0; theta -= (360 / n))
                {
                    Polar2Cart(1, theta, &x, &z);
                    glTexCoord2f(0.5*(1 + x), 0.5*(1 + z));
                    glVertex3d(x, 1, z);
                }
            } glEnd();
        }

        // Sides
        if (sides)
        {
            glBegin(GL_QUAD_STRIP); {
                float x, z;
                for (int theta=0; theta <= 360; theta += (360 / n))
                {
                    Polar2Cart(1, theta, &x, &z);
                    glNormal3f(x, 0, z);
                    glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                    glVertex3d(x, 0, z);
                    glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
                    glVertex3d(x, 1, z);
                }
            } glEnd();
        }

        // Bottom
        if (bottom)
        {
            glBegin(GL_TRIANGLE_FAN); {
                glNormal3f(0, -1, 0);
                glVertex3d(0, -height/2.0f, 0);
                float x, z;
                for (int theta=0; theta <= 360; theta += (360 / n))
                {
                    Polar2Cart(1, theta, &x, &z);
                    glTexCoord2f(0.5*(1 + x), 0.5*(1 + z));
                    glVertex3d(x, 0, z);
                }
            } glEnd();
        }
    } glPopMatrix();
}

void Cylinder::SetTopCenter(float x, float y, float z)
{
    center[0] = x;
    center[1] = y - height / 2.0f;
    center[2] = z;
}

void Cylinder::SetBottomCenter(float x, float y, float z)
{
    center[0] = x;
    center[1] = y + height / 2.0f;
    center[2] = z;
}

Cube::Cube()
{
    sides = (
        left * 32 + right * 16 +
        bottom * 8 + top * 4 +
        back * 2 + forward * 1);
}

void Cube::Draw()
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
		glTranslatef(center[0], center[1], center[2]); // Translate to chosen center
		glScalef(radius*2, radius*2, radius*2); // Apply scaling
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