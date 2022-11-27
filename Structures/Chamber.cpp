#include <functional>

#include "Chamber.hpp"
#include "globals.hpp"

Chamber::Chamber(unsigned char sides)
{
	type = 3;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	UnpackSides(sides);

	Create();
}

Vector3 Chamber::SetCoords(const float v0, const float v1, const float v2, int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	Vector3 coords;
	coords[i0] = (f0 ? -1 : 1) * v0;
	coords[i1] = (f1 ? -1 : 1) * v1;
	coords[i2] = (f2 ? -1 : 1) * v2;
	return coords;
}
void Chamber::FacePointHelper(std::function<Vector3(const float, const float, const float)> BoundSetCoords,
	const float v0, const float v1, const float v2)
{
	auto coords = BoundSetCoords(v0, v1, v2);
	indices.push_back(vertices.size()); triIndices.push_back(vertices.size());
	vertices.push_back(coords);
	normals.push_back(coords);
}
/// @brief Create 1/4 of a face 
void Chamber::FaceHelper(int i0, bool f0, int i1, bool f1, int i2, bool f2, bool hasArm)
{
	Vector3 coords;

	// FIXME generalize for n != 8

	// Ref https://en.cppreference.com/w/cpp/utility/functional/bind
	auto boundSetCoords = std::bind(SetCoords, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, i0, f0, i1, f1, i2, f2);

	// Pre-compute for readability
	float r = 0.5 * padScale;
	float xAtTunnel = sqrt(r*r - tunnelRadius*tunnelRadius);
	float tunnelCorner = sqrt(2)/2 * tunnelRadius;
	float planeCorner = sqrt(2)/2 * padScale * 0.5;
	float tripleCorner = sqrt(3)/3 * padScale * 0.5;

	// Face outside the arm - Start at 90 degrees viewed from +X, move CCW
	if (true) { // 1
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelRadius, 0); // Bottom right
		FacePointHelper(boundSetCoords, planeCorner, planeCorner, 0); // Top point
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom left
	}
	if (true) { // 2
		FacePointHelper(boundSetCoords, tripleCorner, tripleCorner, tripleCorner); // Top left
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom point - coincident with 1's Bottom left
		FacePointHelper(boundSetCoords, planeCorner, planeCorner, 0); // Top right - coincident with 1's Top point
	}
	if (true) { // 3 - like 2 with Y <--> Z
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom point - coincident with 2's Bottom point
		FacePointHelper(boundSetCoords, tripleCorner, tripleCorner, tripleCorner); // Top right - coincident with 2's Top left
		FacePointHelper(boundSetCoords, planeCorner, 0, planeCorner); // Top left - mirror to 2's Top right
	}
	if (true) { // 4 - like 1 with Y <--> Z
		FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Bottom right - coincident with 3's Bottom point
		FacePointHelper(boundSetCoords, planeCorner, 0, planeCorner); // Top point - coincident with 3's Top left
		FacePointHelper(boundSetCoords, xAtTunnel, 0, tunnelRadius); // Bottom left - mirror to 1's Bottom right
	}

	// Face inside the arm, if applicable
	if (!hasArm)
	{ // Start at 90 degrees viewed from +X, move CCW
		if (true) { // A
			FacePointHelper(boundSetCoords, r, 0, 0); // Bottom point
			FacePointHelper(boundSetCoords, xAtTunnel, tunnelRadius, 0); // Top right - coincident with 1's Bottom right
			FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Top left - coincident with 1's Bottom left
		}
		if (true) { // B
			FacePointHelper(boundSetCoords, xAtTunnel, 0, tunnelRadius); // Top left - coincident with 4's Bottom left
			FacePointHelper(boundSetCoords, r, 0, 0); // Bottom point - coincident with A's Bottom point
			FacePointHelper(boundSetCoords, xAtTunnel, tunnelCorner, tunnelCorner); // Top right - coincident with A's Top left
		}
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
	float r = 0.5 * padScale;
	float xAtTunnel = sqrt(r*r - tunnelRadius*tunnelRadius);

	float d = (0.5 - xAtTunnel) / armPanels;
	float thetaD = 360 / tunnelN;

	Vector3 coords;
	float x = xAtTunnel, y, z;
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
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				armIndices.insert(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);

				// Inner edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
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
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
				// FIXME texture
				coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);

				// Outer edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
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

void Chamber::CreateCenterline(int axis, bool flip)
{
	// FIXME should match arm panelling exactly, then handle core differently

	int panels = round(0.5 / tunnelRadius);
	float d = (0.5) / panels;

	std::vector<Vector3> centerline;
	Vector3 coords;
	centerline.push_back(coords); // Center
	float x = tunnelRadius;
	for (int i = 0; i <= panels; i++)
	{
		coords[axis] = (flip ? -1 : 1) * x;
		centerline.push_back(coords);
		x += d;
	}
	centerlines.push_back(centerline);
}

void Chamber::PreCreate()
{
	Structure::PreCreate();
	triIndices.clear();
	quadIndices.clear();
}
void Chamber::Create()
{
	PreCreate();

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

	// Create centerlines
	if (right)   CreateCenterline(0, false);
	if (left)    CreateCenterline(0,  true);
	if (top)     CreateCenterline(1, false);
	if (bottom)  CreateCenterline(1,  true);
	if (forward) CreateCenterline(2, false);
	if (back)    CreateCenterline(2,  true);

	ErrCheck("Chamber::Create\n");

	PostCreate();
}

void Chamber::ApplyNoise(float offset[])
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
			scale = Globals::tunnelNoiseScale;
		}

		float x = baseVertices[i][0] + center[0] + offset[0];
		float y = baseVertices[i][1] + center[1] + offset[1];
		float z = baseVertices[i][2] + center[2] + offset[2];

        auto p = noisePtr->getNoise(x, y, z);
        
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

void Chamber::DrawHelper(std::vector<Vector3> drawVertices)
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
			// fprintf(stdout, "vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]);
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

void Chamber::Draw()
{
	if (Toggles::showNormals) DrawNormals(0.2);

	if (Toggles::debug)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_TEXTURE_2D);
	}

	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		// Perturbed geometry
		if (Toggles::Noise::showPerturbed)
			DrawHelper(vertices);

		// Baseline geometry
		if (Toggles::Noise::showBase)
		{
			if (Toggles::Noise::showPerturbed)
			{ // Use white, transparent wireframe
				glColor4f(1, 1, 1, 0.5);
				glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
				glDisable(GL_TEXTURE_2D);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} // else: Use colored, textured geometry
			DrawHelper(baseVertices);
			if (Toggles::Noise::showPerturbed)
			{
				glPopAttrib();
				glColor4f(1, 1, 1, 1);
			}
		}

		if(Toggles::showCenterlines) DrawCenterlines();
	} glPopMatrix();

	if (Toggles::debug) glPopAttrib();
}