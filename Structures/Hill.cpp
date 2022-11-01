#include "Hill.hpp"

void Hill::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1] - 0.5, center[2]);
        glScalef(1, height, 1);

        // Top
        glBegin(GL_TRIANGLE_FAN); {
            glNormal3f(0, 1, 0);
            glTexCoord2f(0.5, 0.5);
            glVertex3d(0, 1, 0);
            float x, z;
            for (int theta=360; theta >= 0; theta -= (360 / n))
            {
                Polar2Cart(topRadius, theta, &x, &z);
                glTexCoord2f(0.5*(1 + x), 0.5*(1 + z));
                glVertex3d(x, 1, z);
            }
        } glEnd();

 	 	// Sides
        glBegin(GL_QUAD_STRIP); {
            float x, z;
            for (int theta=360; theta >= 0; theta -= (360 / n))
            {
                Polar2Cart(topRadius, theta, &x, &z);
                // FIXME normal should be slanted
                glNormal3f(x, 0, z);
                glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                glVertex3d(x, 1, z);

                Polar2Cart(baseRadius, theta, &x, &z);
                glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
                glVertex3d(x, 0, z);
            }
        } glEnd();

        // Bottom
        // FIXME texture is swirly
        glBegin(GL_TRIANGLE_FAN); {
            glNormal3f(0, -1, 0);
            glTexCoord2f(0.5, 0.5);
            glVertex3d(0, 0, 0);
            float x, z;
            for (int theta=0; theta <= 360; theta += (360 / n))
            {
                Polar2Cart(baseRadius, theta, &x, &z);
                glTexCoord2f(0.5*(1 + x), 0.5*(1 + z));
                glVertex3d(x, 0, z);
            }
        } glEnd();
	} glPopMatrix();
}