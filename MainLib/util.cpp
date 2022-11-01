#include "util.hpp"

/*
 *  Convert polar to Cartesian coordinates
 */
void Polar2Cart(float radius, float theta, float* x_out, float* y_out)
{
   *x_out = radius * Cos(theta);
   *y_out = radius * Sin(theta);
}

Cylinder::Cylinder()
{
}

void Cylinder::Draw()
{
    glPushMatrix(); {
        glTranslatef(center[0], center[1], center[2]);

        // Top
        if (top)
        {
            glBegin(GL_TRIANGLE_FAN); {
                glNormal3f(0, 1, 0);
                glVertex3d(0, height/2.0f, 0);
                float x, z;
                for (int theta=360; theta >= 0; theta -= (360 / n))
                {
                    Polar2Cart(radius, theta, &x, &z);
                    glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                    glVertex3d(x, height/2.0f, z);
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
                    Polar2Cart(radius, theta, &x, &z);
                    glNormal3f(x, 0, z);
                    glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                    glVertex3d(x, -height/2.0f, z);
                    glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
                    glVertex3d(x,  height/2.0f, z);
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
                    Polar2Cart(radius, theta, &x, &z);
                    glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                    glVertex3d(x, -height/2.0f, z);
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