#include "Tunnel.hpp"

void Tunnel::Draw()
{
    glPushMatrix(); {
        glTranslatef(center[0], center[1], center[2]);

        // Arms
        {
            float cyl_height = 0.5f - radius;

            static Cylinder cyl;
            cyl.top = true;
            cyl.bottom = false;
            cyl.radius = radius;
            cyl.height = cyl_height;
            cyl.SetTopCenter(0, 0.5f, 0);

            if (right)
            {
                glPushMatrix(); {
                    glRotatef(90, Z_AXIS);
                    cyl.Draw();
                } glPopMatrix();
            }

            if (left)
            {
                glPushMatrix(); {
                    glRotatef(-90, Z_AXIS);
                    cyl.Draw();
                } glPopMatrix();
            }

            if (top)
            {
                cyl.Draw();
            }

            if (bottom)
            {
                glPushMatrix(); {
                    glRotatef(180, Z_AXIS);
                    cyl.Draw();
                } glPopMatrix();
            }

            if (forward)
            {
                glPushMatrix(); {
                    glRotatef(90, X_AXIS);
                    cyl.Draw();
                } glPopMatrix();
            }

            if (back)
            {
                glPushMatrix(); {
                    glRotatef(-90, X_AXIS);
                    cyl.Draw();
                } glPopMatrix();
            }
        }

        // Core
        {
            static Cube core;
            core.radius = radius;
            core.Draw();
        }
    } glPopMatrix();
}