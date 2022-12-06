#include "Mine.hpp"
#include "globals.hpp"
#include "Objects.hpp"
#include "Shaders.hpp"

Mine::Mine()
{
	type = 5;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	// Padding between edge of chamber and edge of cell
	float padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

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

	// Can't just call Chamber::Create because it includes PostCreate
	{
		// Faces
		if (true)
		{
			CreateFace(0, false, 1, false, 2, false, right);
			CreateFace(1, false, 2, false, 0, false, top);
			CreateFace(2, false, 0, false, 1, false, forward);
			CreateFace(0,  true, 1, false, 2,  true, left);
			CreateFace(1,  true, 2, false, 0,  true, bottom);
			CreateFace(2,  true, 0, false, 1,  true, back);
		}

		// Create centerlines
		centerline.push_back(Vertex(0));
		baseCenterline.push_back(Vertex(0));
		if (right)
		{
			CreateCenterline(0, false);
			endpointRight = centerline.size() - 1;
		}
		if (left)
		{
			CreateCenterline(0,  true);
			endpointLeft = centerline.size() - 1;
		}
		if (top)
		{
			CreateCenterline(1, false);
			endpointTop = centerline.size() - 1;
		}
		if (bottom)
		{
			CreateCenterline(1,  true);
			endpointBottom = centerline.size() - 1;
		}
		if (forward)
		{
			CreateCenterline(2, false);
			endpointForward = centerline.size() - 1;
		}
		if (back)
		{
			CreateCenterline(2,  true);
			endpointBack = centerline.size() - 1;
		}

		ErrCheck("Chamber::Create");
	}

	// Need the color vector to be the same length as the other vectors for the sake of indexing
	Vector3 grey({0.5, 0.5, 0.5});
	for (std::size_t i = 0; i < vertices.size(); i++)
	{
		OBJcolors.push_back(grey);
	}

	// Copy over OBJ values
	for (auto& face : Objects::Mine->OBJfaces)
	{
        for (int i = 0; i < 3; i ++)
        {
            OBJindices.push_back(vertices.size());
            vertices.push_back(Objects::Mine->OBJvertices[face.vIdxs[i]] * (1.0 / Objects::Mine->scale));
            normals.push_back(Objects::Mine->OBJnormals[face.nIdxs[i]]);
            OBJcolors.push_back(Objects::Mine->OBJcolors[face.cIdxs[i]]);
        }
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

	PushShader(Shader::fixedPipeline);
	glPushMatrix();
	glTranslatef(center.x, center.y, center.z);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);


    // Perturbed geometry
    if (Toggles::Noise::showPerturbed) DrawHelper(vertices, normals);
    // Only show base if not showing perturbed, too cluttered to show both
    else if (Toggles::Noise::showBase) DrawHelper(baseVertices, baseNormals);


	glPopAttrib();

	glPopMatrix();
	PopShader();

	ErrCheck("Mine::Draw");
}