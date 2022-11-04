#include "Corner.hpp"
#include "Structure.hpp"

Corner::Corner()
{
}

void Corner::SetRotateAndScale()
{
	if (surroundings.x >= surroundings.y && surroundings.x >= surroundings.z)
	{
		if (surroundings.y >= surroundings.z)
		{
			// x >= y >= z - Matches default
			rotate[0] = 0; rotate[1] = 0; rotate[2] = 0;
			scale[0] *= 1; scale[1] *= 1; scale[2] *= 1;
		}
		else
		{
			// x >= z >= y
			rotate[0] = 90; rotate[1] = 0; rotate[2] = 0;
			scale[0] *= 1; scale[1] *= 1; scale[2] *= -1;
		}
	}
	else if (surroundings.y >= surroundings.x && surroundings.y >= surroundings.z)
	{
		if (surroundings.z >= surroundings.x)
		{
			// y >= z >= x
			rotate[0] = 0; rotate[1] = 90; rotate[2] = 90;
			scale[0] *= 1; scale[1] *= 1; scale[2] *= 1;
		}
		else
		{
			// y >= x >= z
			rotate[0] = 0; rotate[1] = 0; rotate[2] = 90;
			scale[0] *= 1; scale[1] *= -1; scale[2] *= 1;
		}
	}
	else if (surroundings.z >= surroundings.x && surroundings.z >= surroundings.y)
	{
		if (surroundings.x >= surroundings.y)
		{
			// z >= x >= y
			rotate[0] = -90; rotate[1] = 0; rotate[2] = -90;
			scale[0] *= 1; scale[1] *= 1; scale[2] *= 1;
		}
		else
		{
			// z >= y >= x
			rotate[0] = 0; rotate[1] = -90; rotate[2] = 0;
			scale[0] *= 1; scale[1] *= 1; scale[2] *= -1;
		}
	}
	else
	{
		Fatal(999, "SetRotateAndScale fell through");
	}
}

SideType GetNodeType(int x, int y, int z)
{
	// FIXME need to get the neighbor
	Structure structure;

	return structure.sideType;
}

void DrawClosed(int n, float radius)
{
	int d = 360 / n;
	float x, z, r0, r1, y0, y1;

    glPushMatrix(); {
        // FIXME radius doubled just for visibility
        glScalef(radius*2, radius*2, radius*2);

        // Draw quarter sphere, using degenerate quads for the caps (ref Ex. 8)
        for (float phi = 0; phi < 90; phi += d)
        {
            Polar2Cart(1, phi, &r0, &y0);
            Polar2Cart(1, phi + d, &r1, &y1);
            // fprintf(stdout, "slice %f: (%f, %f), (%f, %f)\n", phi, r0, y0, r1, y1);

            glBegin(GL_QUAD_STRIP);
            for (int theta = 0; theta <= 90; theta += d)
            {
                // FIXME texture
                Polar2Cart(r0, theta, &x, &z);
                glNormal3d(x, y0, z);
                glVertex3d(x, y0, z);
                // fprintf(stdout, "0: (%f, %f, %f)", x, y0, z);
                Polar2Cart(r1, theta, &x, &z);
                glNormal3d(x, y1, z);
                glVertex3d(x, y1, z);
                // fprintf(stdout, "   1: (%f, %f, %f)\n", x, y1, z);
            }
            glEnd();
        }
    } glPopMatrix();
}
void DrawOneTunnel(int n, float radius)
{
    glPushMatrix(); {
        // FIXME radius doubled just for visibility
        glScalef(0.5, radius*2, radius*2);

        glBegin(GL_QUAD_STRIP); {
            float y, z;
            for (int theta=0; theta <= 90; theta += (360 / n))
            {
                Polar2Cart(1, theta, &y, &z);
                glNormal3f(0, y, z);
                glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
                glVertex3d(1, y, z);
                glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
                glVertex3d(0, y, z);
            }
        } glEnd();
    } glPopMatrix();
}
void DrawTwoTunnels(int n, float radius)
{
	// FIXME
	DrawOneTunnel(n, radius);
	glPushMatrix(); glRotatef(90, Z_AXIS); DrawOneTunnel(n, radius); glPopMatrix();
}
void DrawThreeTunnels(int n, float radius)
{
	// FIXME
	DrawOneTunnel(n, radius);
	glPushMatrix(); glRotatef(90, Y_AXIS); glRotatef(90, Z_AXIS); DrawOneTunnel(n, radius); glPopMatrix();
	glPushMatrix(); glRotatef(-90, Z_AXIS); glRotatef(-90, Y_AXIS); DrawOneTunnel(n, radius); glPopMatrix();
}

void Corner::UpdateConnections()
{
	//Surroundings surroundings = {0,0,0, 0,0,0};
    /*
	// Handle adjacent nodes
	surroundings.x = (int)GetNodeType(1, 0, 0);
	surroundings.y = (int)GetNodeType(0, 1, 0);
	surroundings.z = (int)GetNodeType(0, 0, 1);
	// Handle diagonal nodes
	surroundings.xy = (surroundings.x == 2 && surroundings.y == 2) ? (int)GetNodeType(1, 1, 0) : 0;
	surroundings.xz = (surroundings.x == 2 && surroundings.y == 2) ? (int)GetNodeType(1, 0, 1) : 0;
	surroundings.yz = (surroundings.x == 2 && surroundings.y == 2) ? (int)GetNodeType(0, 1, 1) : 0;
    */
	
	//int key[2] = {surroundings.sqrMagnitude(), 0};

	SetRotateAndScale();

	glPushMatrix(); {
		glScalef(scale[0], scale[1], scale[2]);
		glRotatef(rotate[0], X_AXIS);
		glRotatef(rotate[1], Y_AXIS);
		glRotatef(rotate[2], Z_AXIS);

		// TODO need a second key to handle diagonals eventually
		switch(surroundings.sqrMagnitude())
		{
		case 0:
			DrawClosed(n, radius);
			break;
		case 1:
			DrawOneTunnel(n, radius);
			break;
		case 2:
			DrawTwoTunnels(n, radius);
			break;
		case 3:
			DrawThreeTunnels(n, radius);
			break;
		// TODO handle chambers, too
		default:
			Fatal(999, "Unknown connection %d\n", surroundings.sqrMagnitude());
		}
	} glPopMatrix();

	// FIXME
	/*
	meshFilter.transform.localEulerAngles = ans.euler;
	meshFilter.transform.localScale = ans.scale;
	*/
}

void Corner::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		glDisable(GL_TEXTURE_2D);
        glColor4f(0.75 + scale[0]*.25, 0.75 + scale[1]*.25, 0.75 + scale[2]*.25, 0.5);

        // FIXME shouldn't recalculate every time
        UpdateConnections();

		glEnable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 1);
    } glPopMatrix();
}
