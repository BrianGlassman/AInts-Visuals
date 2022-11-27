#include "Tunnel.hpp"

#include "globals.hpp"

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

	// Create centerlines
	if (right)   CreateCenterline(0, false);
	if (left)    CreateCenterline(0,  true);
	if (top)     CreateCenterline(1, false);
	if (bottom)  CreateCenterline(1,  true);
	if (forward) CreateCenterline(2, false);
	if (back)    CreateCenterline(2,  true);

	PostCreate();
}

void Tunnel::CreateCenterline(int axis, bool flip)
{
	int panels = 4; // FIXME generalize
	float d = (0.5 - radius) / panels;

	std::vector<Vector3> centerline;
	Vector3 coords;
	centerline.push_back(coords); // Center
	float x = radius;
	for (int i = 0; i <= panels; i++)
	{
		coords[axis] = (flip ? -1 : 1) * x;
		centerline.push_back(coords);
		x += d;
	}
	centerlines.push_back(centerline);
}

void Tunnel::ApplyNoise(float offset[])
{
	// Apply to each corner
    float newOffset[] = {offset[0] + center[0], offset[1] + center[1], offset[2] + center[2]};
	for (auto&& corner : corners)
	{
		corner.ApplyNoise(newOffset);
	}

	// Apply to the centerline
	for (unsigned int i = 0; i < baseCenterlines.size(); i++)
	{
		for (unsigned int j = 0; j < baseCenterlines[i].size(); j++)
		{
			float x = baseCenterlines[i][j].x + center.x + offset[0];
			float y = baseCenterlines[i][j].y + center.y + offset[1];
			float z = baseCenterlines[i][j].z + center.z + offset[2];

        	auto p = noisePtr->getNoise(x, y, z);

			centerlines[i][j].x = baseCenterlines[i][j].x + p[0]*noiseScale;
			centerlines[i][j].y = baseCenterlines[i][j].y + p[1]*noiseScale;
			centerlines[i][j].z = baseCenterlines[i][j].z + p[2]*noiseScale;
		}
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

		if(Toggles::showCenterlines) DrawCenterlines();
	} glPopMatrix();
}