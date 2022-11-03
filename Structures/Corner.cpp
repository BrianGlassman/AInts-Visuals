#include "Corner.hpp"
#include "Structure.hpp"

struct EulerAndScale {
	float euler[3];
	float scale[3];
};

Corner::Corner()
{
}

EulerAndScale CalculateEulerAndScale(Surroundings surroundings)
{
	float euler[3], scale[3];
	if (surroundings.x >= surroundings.y && surroundings.x >= surroundings.z)
	{
		if (surroundings.y >= surroundings.z)
		{
			// x >= y >= z - Matches default
			euler[0] = 0; euler[1] = 0; euler[2] = 0;
			scale[0] = 1; scale[1] = 1; scale[2] = 1;
		}
		else
		{
			// x >= z >= y
			euler[0] = 90; euler[1] = 0; euler[2] = 0;
			scale[0] = 1; scale[1] = 1; scale[2] = -1;
		}
	}
	else if (surroundings.y >= surroundings.x && surroundings.y >= surroundings.z)
	{
		if (surroundings.z >= surroundings.x)
		{
			// y >= z >= x
			euler[0] = 0; euler[1] = 90; euler[2] = 90;
			scale[0] = 1; scale[1] = 1; scale[2] = 1;
		}
		else
		{
			// y >= x >= z
			euler[0] = 0; euler[1] = 0; euler[2] = 90;
			scale[0] = 1; scale[1] = -1; scale[2] = 1;
		}
	}
	else if (surroundings.z >= surroundings.x && surroundings.z >= surroundings.y)
	{
		if (surroundings.x >= surroundings.y)
		{
			// z >= x >= y
			euler[0] = -90; euler[1] = 0; euler[2] = -90;
			scale[0] = 1; scale[1] = 1; scale[2] = 1;
		}
		else
		{
			// z >= y >= x
			euler[0] = 0; euler[1] = -90; euler[2] = 0;
			scale[0] = 1; scale[1] = 1; scale[2] = -1;
		}
	}
	else
	{
		Fatal(999, "CalculateEulerAndScale fell through");
	}

	EulerAndScale ans;
	memcpy(ans.euler, euler, 3*sizeof(float));
	memcpy(ans.scale, scale, 3*sizeof(float));
	return ans;
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
        glScalef(radius, radius, radius);

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
	glPushMatrix(); glRotatef(90, Z_AXIS); DrawOneTunnel(n, radius); glPopMatrix();
	glPushMatrix(); glRotatef(-90, Y_AXIS); DrawOneTunnel(n, radius); glPopMatrix();
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
	EulerAndScale ans = CalculateEulerAndScale(surroundings);

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
        glColor3f(0.5 + center[0], 0.5 + center[1], 0.5 + center[2]);

        // FIXME shouldn't recalculate every time
        UpdateConnections();

        glColor3f(1, 1, 1);
    } glPopMatrix();
}
