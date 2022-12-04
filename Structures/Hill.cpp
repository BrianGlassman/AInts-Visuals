#include "Hill.hpp"

#include "globals.hpp"
#include "Shaders.hpp"

Hill::Hill()
{
    type = 6;
    sideType = SideType::hill;
    noiseScale = Globals::tunnelNoiseScale;

    Create();
}

void Hill::CreateSides()
{
    float d = 360 / n;
    float dy = height / panels;
    float lowerR, upperR;
    float x, y = -0.5, z;
    float N = (baseRadius - topRadius) / height; // d/dy of normal vector
    for (int i = 0; i < panels; i++)
    {
        lowerR = (topRadius - baseRadius) * ((y + 0.5) / height) + baseRadius;
        upperR = (topRadius - baseRadius) * ((y + dy + 0.5) / height) + baseRadius;
        for (float theta = 0; theta < 360; theta += d)
        {
            // --- Lower left triangle ---
            // Top left point
            Polar2Cart(upperR, theta, &x, &z);
            z = -z;
            indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
            normals.push_back({x, N*upperR, z});
            vertices.push_back({x, y + dy, z});

            // Bottom left
            Polar2Cart(lowerR, theta, &x, &z);
            z = -z;
            indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
            normals.push_back({x, N*lowerR, z});
            vertices.push_back({x, y, z});

            // Bottom right
            Polar2Cart(lowerR, theta + d, &x, &z);
            z = -z;
            indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
            normals.push_back({x, N*lowerR, z});
            vertices.push_back({x, y, z});

            // --- Upper triangle ---
            // Top left - coincident with Lower's top point
            Polar2Cart(upperR, theta, &x, &z);
            z = -z;
            indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
            normals.push_back({x, N*upperR, z});
            vertices.push_back({x, y + dy, z});

            // Bottom right point - coincident with Lower's bottom right
            Polar2Cart(lowerR, theta + d, &x, &z);
            z = -z;
            indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
            normals.push_back({x, N*lowerR, z});
            vertices.push_back({x, y, z});

            // Top right
            Polar2Cart(upperR, theta + d, &x, &z);
            z = -z;
            indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
            normals.push_back({x, N*upperR, z});
            vertices.push_back({x, y + dy, z});
        }

        y += dy;
    }
}

void Hill::CreateTunnel()
{
    float dy = height / panels;
    float thetaD = 360 / n;
    float x, y = -0.5, z;
    
    for (int i = 0; i < panels; i++)
    {
        for (float theta = 360; theta > 0; theta -= thetaD)
        {
			{ // This column
                Polar2Cart(topRadius, theta, &x, &z);
                z = -z;

                // Lower edge
                indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
                normals.push_back({x, 0, z});
                vertices.push_back({x, y, z});

                // Upper edge
                indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
                normals.push_back({x, 0, z});
                vertices.push_back({x, y + dy, z});
            }

			{ // Next column
                Polar2Cart(topRadius, theta - thetaD, &x, &z);
                z = -z;

                // Upper edge
                indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
                normals.push_back({x, 0, z});
                vertices.push_back({x, y + dy, z});

                // Lower edge
                indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
                normals.push_back({x, 0, z});
                vertices.push_back({x, y, z});
            }
        }
        y += dy;
    }
}

void Hill::CreateCLHelper(std::vector<Vertex> &CLtoUse)
{
    int lastIdx = -1, currentIdx;
    float dy = height / panels;
    float y = -0.5;
    for (int i = 0; i <= panels; i++)
    {
        // Create
        currentIdx = CLtoUse.size();
        Vertex vert(currentIdx, {0, y, 0});

        // Link
        if (lastIdx >= 0)
        {
            CLtoUse[lastIdx].AddNeighbor(currentIdx);
            vert.AddNeighbor(lastIdx);
        }

		// Insert (MUST be after creation and linking)
        CLtoUse.push_back(vert);

        // Increment
        y += dy;
        lastIdx = currentIdx;
    }
}
void Hill::CreateCenterline()
{
    CreateCLHelper(centerline);
    CreateCLHelper(baseCenterline);
    if (bottom) endpointBottom = centerline.size() - 1;
}

void Hill::PreCreate()
{
    Structure::PreCreate();
	triIndices.clear();
	quadIndices.clear();
}
void Hill::Create()
{
    PreCreate();

    CreateSides();
    CreateTunnel();
    CreateCenterline();

    PostCreate();
}

void Hill::DrawHelper(std::vector<Vector3> drawVertices)
{
	// FIXME array-ification can happen in Create, doesn't need to be in Draw
	glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
		// Convert vector of vectors to flat array
		float vertexArray[drawVertices.size() * 3];
		for (unsigned int i = 0; i < drawVertices.size(); i++)
		{
			vertexArray[i*3 + 0] = drawVertices[i][0];
			vertexArray[i*3 + 1] = drawVertices[i][1];
			vertexArray[i*3 + 2] = drawVertices[i][2];
			// printf("vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]);
		}
		float normalArray[normals.size() * 3];
		for (unsigned int i = 0; i < normals.size(); i++)
		{
			normalArray[i*3 + 0] = normals[i][0];
			normalArray[i*3 + 1] = normals[i][1];
			normalArray[i*3 + 2] = normals[i][2];
		}

		if (triIndices.size() > 0)
		{ // Draw with GL_TRIANGLES
			unsigned int triIndexArray[triIndices.size()];
			for (unsigned int i = 0; i < triIndices.size(); i++)
			{
				triIndexArray[i] = triIndices[i];
				// fprintf(stdout, "%d\n", triIndexArray[i]);
			}
			// fprintf(stdout, "%d: %d\n", triIndices.size()-1, triIndexArray[triIndices.size()-1]); // Check for overflow

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			glDrawElements(GL_TRIANGLES, triIndices.size(), GL_UNSIGNED_INT, triIndexArray);
		}

		if (quadIndices.size() > 0)
		{ // Draw with GL_QUADS
			unsigned int quadIndexArray[quadIndices.size()];
			for (unsigned int i = 0; i < quadIndices.size(); i++)
			{
				quadIndexArray[i] = quadIndices[i];
				// fprintf(stdout, "%d\n", quadIndexArray[i]);
			}
			// fprintf(stdout, "%d: %d\n", quadIndices.size()-1, quadIndexArray[quadIndices.size()-1]); // Check for overflow

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			glDrawElements(GL_QUADS, quadIndices.size(), GL_UNSIGNED_INT, quadIndexArray);
		}
	} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
}
void Hill::Draw(bool hasControl)
{
	if (Toggles::showNormals) DrawNormals(0.2);

    if (hasControl)
    {
        if (Toggles::debug)
        {
            PushShader(Shader::fixedPipeline);
            glPushAttrib(GL_ENABLE_BIT); glDisable(GL_TEXTURE_2D);
        }
        else
        {
            PushShader(Shader::threeDshader);
        }
    }


	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		// Perturbed geometry
		if (Toggles::Noise::showPerturbed)
			DrawHelper(vertices);

		// Baseline geometry
		if (Toggles::Noise::showBase)
		{
			if (Toggles::Noise::showPerturbed && hasControl)
			{ // Use white, transparent wireframe
				glColor4f(1, 1, 1, 0.5);
				glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
				glDisable(GL_TEXTURE_2D);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} // else: Use colored, textured geometry

			DrawHelper(baseVertices);

            // Reset if changes were made
			if (Toggles::Noise::showPerturbed && hasControl)
			{
				glPopAttrib();
				glColor4f(1, 1, 1, 1);
			}
		}
	} glPopMatrix();

    if (hasControl)
    {
        if (Toggles::debug) glPopAttrib();
        PopShader();
    }

    ErrCheck("Hill::Draw");
}