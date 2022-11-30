#include <unordered_set>

#include "Structure.hpp"
#include "globals.hpp"
#include "Shaders.hpp"

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
}

std::vector<Vertex>* Structure::getPerturbedCL()
{
	return &centerline;
}
std::vector<Vertex>* Structure::getBaseCL()
{
	return &baseCenterline;
}
std::vector<Vertex>* Structure::getCL()
{
	if (Toggles::Noise::showPerturbed)
		return getPerturbedCL();
	else
		return getBaseCL();
}

void Structure::DrawCenterlines()
{
	PushShader(Shader::fixedPipeline);
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPointSize(7);
	glColor3f(0, 1, 1);

	auto& CLtoUse = *getCL();
	if (CLtoUse.size() == 0)
	{ // Exit early to prevent SegFault
		Fatal(999, "Called DrawCenterLines with no elements\n");
	}

	std::unordered_set<int> closedSet;
	std::unordered_set<int> frontier;
	frontier.insert(0);
	glBegin(GL_POINTS); glVertex3f(CLtoUse[0].x(), CLtoUse[0].y(), CLtoUse[0].z()); glEnd();

	// Depth-first iteration
	while (frontier.size() > 0)
	{
		// Pop the next point to look at
		auto currentIdx = *frontier.begin();
		frontier.erase(frontier.begin());
		auto current = CLtoUse[currentIdx];
		// printf("Current has %lu neighbors\n", current.neighbors.size());

		for (auto&& neighborIdx : current.neighbors)
		{
			auto&& neighbor = CLtoUse[neighborIdx];

			// Draw line from this point to the neighbor
			glBegin(GL_LINES);
			glVertex3f( current.x(),  current.y(),  current.z());
			// printf("current %f, %f, %f\n", current.x(),  current.y(),  current.z());
			glVertex3f(neighbor.x(), neighbor.y(), neighbor.z());
			// printf("neighbor %f, %f, %f\n", neighbor.x(),  neighbor.y(),  neighbor.z());
			glEnd();

			// Add the neighbor to the frontier (if not already in closedSet or frontier)
			if (closedSet.find(neighborIdx) == closedSet.end() && frontier.find(neighborIdx) == frontier.end())
			{
				frontier.insert(neighborIdx);

				// Draw a point
				glBegin(GL_POINTS); glVertex3f(neighbor.x(), neighbor.y(), neighbor.z()); glEnd();
			}

		}
		closedSet.insert(currentIdx);
	}
	
	glColor3f(1, 1, 1);
	glPopAttrib();
	PopShader();

	ErrCheck("Structure::DrawCenterlines\n");
}