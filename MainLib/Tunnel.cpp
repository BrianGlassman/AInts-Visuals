#include "Tunnel.hpp"

void Tunnel::Draw()
{
    glPushMatrix(); {
        glTranslatef(center[0], center[1], center[2]);

        float cyl_height = 0.5f - radius;

        if (right)
        {
            static Cylinder cyl;
            cyl.top = true;
            cyl.bottom = false;
            cyl.radius = radius;
            cyl.height = cyl_height;
            cyl.SetTopCenter(0, 0.5f, 0);
            glPushMatrix(); {
                glRotatef(90, Z_AXIS);
                cyl.Draw();
            } glPopMatrix();
        }

        if (top)
        {
            static Cylinder cyl;
            cyl.top = true;
            cyl.bottom = false;
            cyl.radius = radius;
            cyl.height = cyl_height;
            cyl.SetTopCenter(0, 0.5f, 0);
            cyl.Draw();
        }

        if (bottom)
        {
            static Cylinder cyl;
            cyl.top = false;
            cyl.bottom = true;
            cyl.radius = radius;
            cyl.height = cyl_height;
            cyl.SetBottomCenter(0, -0.5f, 0);
            cyl.Draw();
        }
    } glPopMatrix();
}