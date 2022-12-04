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

	// Reset centerlines
	centerline.clear();
	baseCenterline.clear();

	// Reset endpoints
	  endpointRight = -1;
	   endpointLeft = -1;
	    endpointTop = -1;
	 endpointBottom = -1;
	endpointForward = -1;
	   endpointBack = -1;
}

void Structure::ApplyNoiseCLHelper(unsigned int i, float scale)
{
	Vector3 p = noisePtr->getNoise(baseCenterline[i].coords + center);

	centerline[i].coords = baseCenterline[i].coords + p*scale;
}

void Structure::ApplyNoise()
{
	// Call parent version to apply to the vertices
	Model::ApplyNoise();

	// Apply to the centerline
	for (unsigned int i = 0; i < baseCenterline.size(); i++)
	{
		ApplyNoiseCLHelper(i, noiseScale);
	}
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
		fprintf(stderr, "Called DrawCenterLines with no elements\n");
		glColor3f(1, 1, 1);
		glPopAttrib();
		PopShader();
		return;
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

	ErrCheck("Structure::DrawCenterlines");
}