#include "Structure.hpp"
#include "globals.hpp"

void Structure::UnpackSides(unsigned char sides)
{
	forward = sides & 1;
	back = sides & 2;
	top = sides & 4;
	bottom = sides & 8;
	right = sides & 16;
	left = sides & 32;
}

void Structure::PostCreate()
{
	Model::PostCreate();

	// Copy centerlines to baseCenterlines
	for (auto&& centerline : centerlines)
	{
		std::vector<Vector3> baseCenterline;
		for (auto&& vertex : centerline)
		{
			baseCenterline.push_back(vertex);
		}
		baseCenterlines.push_back(baseCenterline);
	}
}

void Structure::DrawCenterlines()
{
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPointSize(7);
	glColor3f(0, 1, 1);
	auto CLtoUse = (Toggles::Noise::showPerturbed) ? centerlines : baseCenterlines;
	for (auto&& centerline : CLtoUse)
	{
		glBegin(GL_LINE_STRIP);
		for (auto&& vertex : centerline)
		{
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
		glEnd();

		glBegin(GL_POINTS);
		for (auto&& vertex : centerline)
		{
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
		glEnd();
		
	}
	glColor3f(1, 1, 1);
	glPopAttrib();
}