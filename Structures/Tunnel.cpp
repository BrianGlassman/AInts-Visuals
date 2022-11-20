#include "Tunnel.hpp"

Tunnel::Tunnel(unsigned char sides)
{
	type = 2;
	sideType = SideType::tunnel;

	UnpackSides(sides);

	Create();
}

void Tunnel::Create()
{
	PreCreate();

	Corner* corner;
	// Right top front
	corner = &corners[0];
	// FIXME final version should not be offset
	corner->baseScale[0] = 1; corner->baseScale[1] = 1; corner->baseScale[2] = 1;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)forward;

	// Right top back
	corner = &corners[1];
	// FIXME final version should not be offset
	corner->baseScale[0] = 1; corner->baseScale[1] = 1; corner->baseScale[2] = -1;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)back;

	// Right bottom front
	corner = &corners[2];
	// FIXME final version should not be offset
	corner->baseScale[0] = 1; corner->baseScale[1] = -1; corner->baseScale[2] = 1;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)forward;
	
	// Right bottom back
	corner = &corners[3];
	// FIXME final version should not be offset
	corner->baseScale[0] = 1; corner->baseScale[1] = -1; corner->baseScale[2] = -1;
	corner->surroundings.x = (int)right;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)back;
	
	// Left top front
	corner = &corners[4];
	// FIXME final version should not be offset
	corner->baseScale[0] = -1; corner->baseScale[1] = 1; corner->baseScale[2] = 1;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)forward;

	// Left top back
	corner = &corners[5];
	// FIXME final version should not be offset
	corner->baseScale[0] = -1; corner->baseScale[1] = 1; corner->baseScale[2] = -1;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)top;
	corner->surroundings.z = (int)back;

	// Left bottom front
	corner = &corners[6];
	// FIXME final version should not be offset
	corner->baseScale[0] = -1; corner->baseScale[1] = -1; corner->baseScale[2] = 1;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)forward;
	
	// Left bottom back
	corner = &corners[7];
	// FIXME final version should not be offset
	corner->baseScale[0] = -1; corner->baseScale[1] = -1; corner->baseScale[2] = -1;
	corner->surroundings.x = (int)left;
	corner->surroundings.y = (int)bottom;
	corner->surroundings.z = (int)back;

	// FIXME temp override
	// for (auto&& corner : corners)
	// {
	// 	corner.surroundings.x = 1;
	// 	corner.surroundings.y = 0;
	// 	corner.surroundings.z = 1;
	// }

	// Create vertices AFTER setting surroundings
	for (auto&& corner : corners)
	{
		corner.n = n;
		corner.radius = radius;
		corner.Create();
	}

	PostCreate();
}

void Tunnel::ApplyNoise(Noise* noise, float offset[])
{
    float newOffset[] = {offset[0] + center[0], offset[1] + center[1], offset[2] + center[2]};
	for (auto&& corner : corners)
	{
		corner.ApplyNoise(noise, newOffset);
	}
}

void Tunnel::OldDraw()
{
	// Arms
	{
		float cyl_height = 0.5f - radius*0.5;

		static Cylinder cyl;
		cyl.top = true;
		cyl.bottom = false;
		cyl.radius = radius * 0.5;
		cyl.height = cyl_height;
		cyl.SetTopCenter(0, 0.5f, 0);

		if (right)
		{
			glPushMatrix(); {
				glRotatef(-90, Z_AXIS);
				cyl.Draw();
			} glPopMatrix();
		}

		if (left)
		{
			glPushMatrix(); {
				glRotatef(90, Z_AXIS);
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
	core.radius = radius * 0.5;
	core.Draw();

}

void Tunnel::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		// OldDraw();

		for (auto &&corner : corners)
		{
			corner.Draw();
		}
		
	} glPopMatrix();
}