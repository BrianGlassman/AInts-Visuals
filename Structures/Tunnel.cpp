#include "Tunnel.hpp"

Tunnel::Tunnel()
{
	Tunnel(63);
}
Tunnel::Tunnel(unsigned char sides)
{
	type = 2;
	sideType = SideType::tunnel;

	forward = sides & 1;
	back = sides & 2;
	top = sides & 4;
	bottom = sides & 8;
	right = sides & 16;
	left = sides & 32;

	Corner* corner;
	// Right top front
	corner = &corners[0];
	// FIXME final version should not be offset
	corner->center[0] = 0.25; corner->center[1] = 0.25; corner->center[2] = 0.25;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)forward;

	// Right top back
	corner = &corners[1];
	// FIXME final version should not be offset
	corner->center[0] = 0.25; corner->center[1] = 0.25; corner->center[2] = -0.25;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)back;

	// Right bottom front
	corner = &corners[2];
	// FIXME final version should not be offset
	corner->center[0] = 0.25; corner->center[1] = -0.25; corner->center[2] = 0.25;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)forward;
	
	// Right bottom back
	corner = &corners[3];
	// FIXME final version should not be offset
	corner->center[0] = 0.25; corner->center[1] = -0.25; corner->center[2] = -0.25;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)back;
	
	// Left top front
	corner = &corners[0];
	// FIXME final version should not be offset
	corner->center[0] = -0.25; corner->center[1] = 0.25; corner->center[2] = 0.25;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)forward;

	// Left top back
	corner = &corners[1];
	// FIXME final version should not be offset
	corner->center[0] = -0.25; corner->center[1] = 0.25; corner->center[2] = -0.25;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)back;

	// Left bottom front
	corner = &corners[2];
	// FIXME final version should not be offset
	corner->center[0] = -0.25; corner->center[1] = -0.25; corner->center[2] = 0.25;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)forward;
	
	// Left bottom back
	corner = &corners[3];
	// FIXME final version should not be offset
	corner->center[0] = -0.25; corner->center[1] = -0.25; corner->center[2] = -0.25;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)back;
}

void Tunnel::OldDraw()
{
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
	static Cube core;
	core.radius = radius;
	core.Draw();

}

void Tunnel::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		//OldDraw();

		for (auto &&corner : corners)
		{
			corner.Draw();
		}
		
	} glPopMatrix();
}