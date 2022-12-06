#include "Mine.hpp"
#include "globals.hpp"
#include "Shaders.hpp"

Mine::Mine()
{
	type = StructureType::Mine;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	// Padding between edge of chamber and edge of cell
	float padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	printf("not yet\n");
	OBJptr = Objects::MineTopCap;
	printf("Got here\n");
	if (OBJptr == nullptr) Fatal(999, "OBJptr is null\n");

	Create();
}

void Mine::PreCreate()
{
	Chamber::PreCreate();
	OBJindices.clear();
	OBJcolors.clear();
}

void Mine::Create()
{
	PreCreate();

	Chamber::Create(false);

	// Need the color vector to be the same length as the other vectors for the sake of indexing
	Vector3 grey({0.5, 0.5, 0.5});
	for (std::size_t i = 0; i < vertices.size(); i++)
	{
		OBJcolors.push_back(grey);
	}

	// Copy over OBJ values
	// NOTE: OBJ reuses vertices, I do not
	for (auto& face : OBJptr->OBJfaces)
	{
        for (int i = 0; i < 3; i ++)
        {
            OBJindices.push_back(vertices.size());
            vertices.push_back(OBJptr->OBJvertices[face.vIdxs[i]]);
            normals.push_back(OBJptr->OBJnormals[face.nIdxs[i]]);
            OBJcolors.push_back(OBJptr->OBJcolors[face.cIdxs[i]]);
        }
	}

	if (false)
	{ // Join the OBJ geometry to the Chamber geometry
		std::vector<int> edge;
		for (int axis = 0; axis <= 2; axis++)
		{
			// --- Find the furthest points
			float mx = 49.0 / 144;
			std::vector<int> thisEdge;
			float val;
			for (std::size_t i = OBJindices.front() ; i < vertices.size(); i++)
			{
				auto& vertex = vertices.at(i);
				val = abs(vertex[axis]);

				if (val < mx) continue;

				if (val > mx)
				{
					thisEdge.clear();
					mx = val;
					// printf("New mx = %f\n", mx); // NORELEASE
				}

				if (val == mx)
				{
					thisEdge.push_back(i);
					// printf("Edge vertex (i = %d) %f, %f, %f\n", i, vertex.x, vertex.y, vertex.z); // NORELEASE
				}
			}

			edge.insert(edge.begin(), thisEdge.begin(), thisEdge.end());
		}

		// --- Move the edge vertices to the nearest Chamber arm vertices
		for (auto& i : edge)
		{
			auto& vertex = vertices.at(i);
			// printf("vertices_i = %d: before %f, %f, %f\n", i, vertex.x, vertex.y, vertex.z); // NORELEASE

			float bestSqrMag = 999;
			Vector3 bestCoords;
			for (int ci = 0; ci < OBJindices.front(); ci++)
			{
				auto& cvert = vertices.at(ci);
				if ((cvert - vertex).SqrMag() < bestSqrMag)
				{
					bestSqrMag = (cvert - vertex).SqrMag();
					bestCoords = cvert;
				}
			}
			vertex = bestCoords;

			// printf("after %f, %f, %f\n", vertex.x, vertex.y, vertex.z); // NORELEASE
		}
		printf("Done\n");
	}

	PostCreate();

	ErrCheck("Mine::Create");
}

void Mine::DrawHelper(std::vector<Vector3> drawVertices, std::vector<Vector3> drawNormals)
{
	// OPTIM array-ification can happen in Create, doesn't need to be in Draw // NORELEASE
	glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); glEnableClientState(GL_COLOR_ARRAY); {
		// Convert vector of vectors to flat array
		float vertexArray[drawVertices.size() * 3];
		for (unsigned int i = 0; i < drawVertices.size(); i++)
		{
			vertexArray[i*3 + 0] = drawVertices[i][0];
			vertexArray[i*3 + 1] = drawVertices[i][1];
			vertexArray[i*3 + 2] = drawVertices[i][2];
			if (std::find(OBJindices.begin(), OBJindices.end(), i) != OBJindices.end())
			{
				// fprintf(stdout, "vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]); // NORELEASE
			}
		}
		float normalArray[drawNormals.size() * 3];
		for (unsigned int i = 0; i < drawNormals.size(); i++)
		{
			normalArray[i*3 + 0] = drawNormals[i][0];
			normalArray[i*3 + 1] = drawNormals[i][1];
			normalArray[i*3 + 2] = drawNormals[i][2];
		}

		// colorArray only used for OBJ vertices
        float colorArray[OBJcolors.size() * 3];
        for (unsigned int i = 0; i < OBJcolors.size(); i++)
        {
            colorArray[i*3 + 0] = OBJcolors[i][0];
            colorArray[i*3 + 1] = OBJcolors[i][1];
            colorArray[i*3 + 2] = OBJcolors[i][2];
        }

		if (OBJindices.size() > 0)
		{ // Draw with GL_TRIANGLES
			unsigned int triIndexArray[OBJindices.size()];
			for (unsigned int i = 0; i < OBJindices.size(); i++)
			{
				triIndexArray[i] = OBJindices[i];
				// fprintf(stdout, "%d\n", triIndexArray[i]); // NORELEASE
			}
			// fprintf(stdout, "%d: %d\n", OBJindices.size()-1, triIndexArray[triIndices.size()-1]); // Check for overflow // NORELEASE

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
            glColorPointer(3, GL_FLOAT, 0, colorArray);
			glDrawElements(GL_TRIANGLES, OBJindices.size(), GL_UNSIGNED_INT, triIndexArray);
		}
	} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_COLOR_ARRAY);
}

void Mine::Draw(bool hasControl)
{
	// Chamber::Draw();

	PushShader(Shader::threeDshader);
	glPushAttrib(GL_ENABLE_BIT);
	glPushMatrix(); {
		glTranslatef(center.x, center.y, center.z);

		// Perturbed geometry
		if (Toggles::Noise::showPerturbed) DrawHelper(vertices, normals);
		// Only show base if not showing perturbed, too cluttered to show both
		else if (Toggles::Noise::showBase) DrawHelper(baseVertices, baseNormals);

	} glPopMatrix();
	glPopAttrib();
	PopShader();

	ErrCheck("Mine::Draw");
}