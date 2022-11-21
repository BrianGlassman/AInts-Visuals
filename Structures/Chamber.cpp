#include <functional>

#include "Chamber.hpp"

// FIXME should be able to combine padding into radius
float padding = 0.1; // Padding between edge of chamber and edge of cell
float padScale = 1 - 2*padding; // Scale factor to account for padding

// FIXME these should be linked, not hard-coded
float tunnelNoiseScale = 0.3;
int tunnelN = 8;
int armPanels = 1; // Shouldn't be the same as the tunnel value, but should be related
float tunnelRadius = 0.1f;
// endFixme

Chamber::Chamber(unsigned char sides)
{
	type = 3;
	sideType = SideType::chamber;
	noiseScale = 0.3;

	UnpackSides(sides);

	Create();
}

std::vector<float> SetCoords(const float v0, const float v1, const float v2, int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	std::vector<float> coords(3);
	coords[i0] = (f0 ? -1 : 1) * v0;
	coords[i1] = (f1 ? -1 : 1) * v1;
	coords[i2] = (f2 ? -1 : 1) * v2;
	return coords;
}
void Chamber::FaceHelper(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm)
{
	std::vector<float> coords(3);
	int idx = vertices.size();

	float useTR;
	if (hasArm)
	{
		// Make an opening for the arm
		useTR = tunnelRadius;
	}
	else
	{
		// Use degenerate quads to close the face
		useTR = 0;
	}

	// FIXME generalize for n != 8

	// Ref https://en.cppreference.com/w/cpp/utility/functional/bind
	auto coordSetter = std::bind(SetCoords, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, i0, f0, i1, f1, i2, f2);

	{ // Top cap-to-center
		coords = coordSetter(0.5 * padScale, useTR*sqrt(2)/2, useTR*sqrt(2)/2); // Bottom left
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
		coords = coordSetter(0.5 * padScale, useTR, 0); // Bottom right
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
		coords = coordSetter(sqrt(2) / 4 * padScale, sqrt(2) / 4 * padScale, 0); // Top right
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
		coords = coordSetter(sqrt(3) / 6 * padScale, sqrt(3) / 6 * padScale, sqrt(3) / 6 * padScale); // Top left
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
	}

	{ // Bottom cap-to-center
		coords = coordSetter(sqrt(3) / 6 * padScale, sqrt(3) / 6 * padScale, sqrt(3) / 6 * padScale); // Triple center
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
		coords = coordSetter(sqrt(2) / 4 * padScale, 0, sqrt(2) / 4 * padScale); // Bottom edge, center
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
		coords = coordSetter(0.5 * padScale, 0, useTR); // Bottom edge, arm
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
		coords = coordSetter(0.5 * padScale, useTR*sqrt(2)/2, useTR*sqrt(2)/2); // Arm center
		indices.push_back(vertices.size()); vertices.push_back(coords); normals.push_back(coords);
	}
}
void Chamber::CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm)
{
	FaceHelper(i0, f0, i1, f1, i2, f2, hasArm);
	FaceHelper(i0, f0, i2, f2, i1, !f1, hasArm);
	FaceHelper(i0, f0, i2, !f2, i1, f1, hasArm);
	FaceHelper(i0, f0, i1, !f1, i2, !f2, hasArm);

	if (hasArm)
	{
		CreateArm(i0, f0, i1, f1, i2, f2);
	}
}
void Chamber::CreateArm(int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	float d = padding / armPanels;
	float thetaD = 360 / tunnelN;

	std::vector<float> coords(3);
	float x = (0.5 - padding), y, z;
	for (int i = 0; i < armPanels; i++)
	{
		for (float theta = 0; theta < 360; theta += thetaD)
		{
			// Top-left quadrant, moving CCW
			{ // This row
				Polar2Cart(tunnelRadius, theta, &z, &y);
				z = -z;
				coords[i0] = 0;
				coords[i1] = f1 ? -y : y;
				coords[i2] = f2 ? -z : z;

				// Outer edge
				indices.push_back(vertices.size());
				armIndices.insert(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);

				// Inner edge
				indices.push_back(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);
			}
			{ // Next row
				Polar2Cart(tunnelRadius, theta + thetaD, &z, &y);
				z = -z;
				coords[i0] = 0;
				coords[i1] = f1 ? -y : y;
				coords[i2] = f2 ? -z : z;
				// Inner edge
				indices.push_back(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);

				// Outer edge
				indices.push_back(vertices.size());
				armIndices.insert(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);
			}
		}

		x += d;
	}
}
void Chamber::Create()
{
	PreCreate();

	panelWidth = (1.0 - 2 * padding) / panels;
	// fprintf(stdout, "panelWidth = %f\n", panelWidth);

	// Note: flip i0 to change winding direction

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

	ErrCheck("Chamber::Create\n");

	PostCreate();
}

void Chamber::ApplyNoise(Noise* noise, float offset[])
{
	// Use different noise scaling for arm portion versus main chamber
	float scale;
	for (unsigned int i = 0; i < baseVertices.size(); i++)
	{
		if (armIndices.find(i) == armIndices.end())
		{
			scale = noiseScale;
		}
		else
		{
			scale = tunnelNoiseScale;
		}

		float x = baseVertices[i][0] + center[0] + offset[0];
		float y = baseVertices[i][1] + center[1] + offset[1];
		float z = baseVertices[i][2] + center[2] + offset[2];

        auto p = noise->getNoise(x, y, z);
        
		vertices[i][0] = baseVertices[i][0] + p[0]*scale;
        vertices[i][1] = baseVertices[i][1] + p[1]*scale;
        vertices[i][2] = baseVertices[i][2] + p[2]*scale;

		if (i == 0)
		{
			// fprintf(stdout, "base (%f, %f, %f), noise (%f, %f, %f)\n",
			// 	baseVertices[i][0], baseVertices[i][1], baseVertices[i][2],
			// 	p[0], p[1], p[2]);
		}
    }
}

void Chamber::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		// FIXME array-ification can happen in Create, doesn't need to be in Draw
		glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
			// Convert vector of vectors to flat array
			float vertexArray[vertices.size() * 3];
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				vertexArray[i*3 + 0] = vertices[i][0];
				vertexArray[i*3 + 1] = vertices[i][1];
				vertexArray[i*3 + 2] = vertices[i][2];
				// fprintf(stdout, "vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]);
			}
			float normalArray[normals.size() * 3];
			for (unsigned int i = 0; i < normals.size(); i++)
			{
				normalArray[i*3 + 0] = normals[i][0];
				normalArray[i*3 + 1] = normals[i][1];
				normalArray[i*3 + 2] = normals[i][2];
			}

			unsigned int indexArray[indices.size()];
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				indexArray[i] = indices[i];
				// fprintf(stdout, "%d\n", indexArray[i]);
			}
			// fprintf(stdout, "%d: %d\n", indices.size()-1, indexArray[indices.size()-1]); // Check for overflow

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indexArray);
		} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
	} glPopMatrix();
}