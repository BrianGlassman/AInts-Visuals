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

void Structure::PreCreate()
{
	Model::PreCreate();
	centerline.clear();
	baseCenterline.clear();
	CLbreaks.clear(); CLbreaks.push_back(0);
}

void Structure::PostCreate()
{
	CLbreaks.push_back(centerline.size());
	Model::PostCreate();

	// Copy centerlines to baseCenterlines
	for (auto&& vertex : centerline)
	{
		baseCenterline.push_back(vertex);
	}
}

void Structure::CLVertexHelper(GLenum mode, std::vector<Vertex> CLtoUse)
{
	for (unsigned int breakIdx = 0; breakIdx < CLbreaks.size() - 1; breakIdx++)
	{
		glBegin(mode);
		// FIXME assumes that all lines connect at first point
		glVertex3f(CLtoUse[0].coords.x, CLtoUse[0].coords.y, CLtoUse[0].coords.z);
		for (unsigned int i = CLbreaks[breakIdx]; i < CLbreaks[breakIdx+1]; i++)
		{
			glVertex3f(CLtoUse[i].coords.x, CLtoUse[i].coords.y, CLtoUse[i].coords.z);
		}
		glEnd();
	}

	ErrCheck("Structure::CLVertexHelper\n");
}
void Structure::DrawCenterlines()
{
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPointSize(7);
	glColor3f(0, 1, 1);

	auto CLtoUse = (Toggles::Noise::showPerturbed) ? centerline : baseCenterline;
	CLVertexHelper(GL_LINE_STRIP, CLtoUse);
	CLVertexHelper(GL_POINTS, CLtoUse);
	
	glColor3f(1, 1, 1);
	glPopAttrib();
}