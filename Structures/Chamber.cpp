#include "Chamber.hpp"

// FIXME should be able to combine padding into radius
float padding = 0.1; // Padding between edge of chamber and edge of cell

// FIXME these should be linked, not hard-coded
float tunnelNoiseScale = 0.07;
int tunnelN = 8;
int armPanels = 1; // Shouldn't be the same as the tunnel value, but should be related
float tunnelRadius = 0.1f;
// endFixme

Chamber::Chamber(unsigned char sides)
{
	type = 3;
	sideType = SideType::chamber;
	noiseScale = 0.49;

	UnpackSides(sides);

	Create();
}

void Chamber::CreateCorner(int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	std::vector<float> norm(3);
	norm[i0] = f0 ? -1 : 1; norm[i1] = f1 ? -1 : 1; norm[i2] = f2 ? -1 : 1;

	std::vector<float> coords(3);
	int idx = vertices.size();
	float x, y, z;
	// Draw a degenerate quad, repeated the top point

	x = (f0 ? -1 : 1) * ((0.5 - padding) - panelWidth);
	y = (f1 ? -1 : 1) * ((0.5 - padding) - panelWidth);
	z = (f2 ? -1 : 1) * ((0.5 - padding) - panelWidth);

	// Top point
	coords[i0] = x;
	coords[i1] = f1 ? -(0.5 - padding) : (0.5 - padding);
	coords[i2] = z;
	vertices.push_back(coords);
	vertices.push_back(coords);
	// Bottom left
	coords[i0] = x;
	coords[i1] = y;
	coords[i2] = f2 ? -(0.5 - padding) : (0.5 - padding);
	vertices.push_back(coords);
	// Bottom right
	coords[i0] = f0 ? -(0.5 - padding) : (0.5 - padding);
	coords[i1] = y;
	coords[i2] = z;
	vertices.push_back(coords);

	for (int i = 0; i < 4; i++)
	{
		normals.push_back(std::vector<float> {
			f0 ? -1.0f : 1.0f,
			f1 ? -1.0f : 1.0f,
			f2 ? -1.0f : 1.0f});
		indices.push_back(idx);
		idx++;
	}
}
void Chamber::CreateEdge(int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	// fprintf(stdout, "Start CreateEdge, %d vertices\n", vertices.size());

	std::vector<float> norm(3);
	norm[i0] = 0; norm[i1] = f1 ? -1 : 1; norm[i2] = f2 ? -1 : 1;

	std::vector<float> coords(3);
	int idx = vertices.size();
	float thisX, nextX, y, z;
	for (int xi = 1; xi < panels - 1; xi++)
	{
		thisX = -(0.5 - padding) + panelWidth*xi;
		nextX = thisX + panelWidth;
		// fprintf(stdout, "x = %f", x);

		//--- For now, make diagonal quads ---

		// Bottom left
		y = (0.5 - padding) - panelWidth; z = (0.5 - padding);
		coords[i0] = f0 ? -thisX : thisX;
		coords[i1] = f1 ? -y : y;
		coords[i2] = f2 ? -z : z;
		vertices.push_back(coords);
		// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
		// Bottom right
		coords[i0] = f0 ? -nextX : nextX;
		vertices.push_back(coords);
		// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
		// Top right
		y = (0.5 - padding); z = (0.5 - padding) - panelWidth;
		coords[i1] = f1 ? -y : y;
		coords[i2] = f2 ? -z : z;
		vertices.push_back(coords);
		// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
		// Top left
		coords[i0] = f0 ? -thisX : thisX;
		vertices.push_back(coords);
		// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);

		for (int i = 0; i < 4; i++)
		{
			normals.push_back(norm);
			indices.push_back(idx);
			idx++;
		}
	}
	// fprintf(stdout, "End CreateEdge, %d vertices\n", vertices.size());
}
void Chamber::CreateFace(int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
	// fprintf(stdout, "Start CreateFace, %d vertices\n", vertices.size());

	std::vector<float> norm(3);
	norm[i0] = 0; norm[i1] = 0; norm[i2] = f2 ? -1 : 1;

	std::vector<float> coords(3);
	int idx = vertices.size();
	float x, y, z = f2 ? -(0.5 - padding) : (0.5 - padding);
	coords[i2] = z;
	for (int xi = 1; xi < panels - 1; xi++)
	{
		x = -(0.5 - padding) + panelWidth*xi;
		// fprintf(stdout, "x = %f", x);
		for (int yi = 1; yi < panels - 1; yi++)
		{
			y = -(0.5 - padding) + panelWidth*yi;
			// fprintf(stdout, "y = %f", y);

			// Bottom left
			coords[i0] = f0 ? -x : x; coords[i1] = f1 ? -y : y;
			vertices.push_back(coords);
			// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
			// Bottom right
			coords[i0] = (f0 ? -1 : 1) * (x + panelWidth);
			vertices.push_back(coords);
			// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
			// Top right
			coords[i1] = (f1 ? -1 : 1) * (y + panelWidth);
			vertices.push_back(coords);
			// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
			// Top left
			coords[i0] = f0 ? -x : x;
			vertices.push_back(coords);
			// fprintf(stdout, "Adding vertex (%f, %f, %f)\n", coords[0], coords[1], coords[2]);

			for (int i = 0; i < 4; i++)
			{
				normals.push_back(norm);
				indices.push_back(idx);
				idx++;
			}
		}
	}
	// fprintf(stdout, "End CreateFace, %d vertices\n", vertices.size());
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
		CreateFace(0, false, 1, false, 2, false); // Front
		CreateFace(1, false, 2, false, 0, false); // Right
		CreateFace(2, false, 0, false, 1, false); // Top
		CreateFace(0,  true, 1, false, 2,  true); // Back
		CreateFace(1,  true, 2, false, 0,  true); // Left
		CreateFace(2,  true, 0, false, 1,  true); // Bottom
	}

	// Edges
	if (true)
	{
		// X-major
		CreateEdge(0, false, 1, false, 2, false); // Top Front
		CreateEdge(0,  true, 1,  true, 2, false); // Bottom Front
		CreateEdge(0, false, 2,  true, 1, false); // Top Back
		CreateEdge(0,  true, 2,  true, 1,  true); // Bottom Back

		// Y-major
		CreateEdge(1,  true, 0, false, 2, false);
		CreateEdge(1, false, 0,  true, 2, false);
		CreateEdge(1,  true, 2,  true, 0, false);
		CreateEdge(1, false, 2,  true, 0,  true);

		// Z-major
		CreateEdge(2, false, 0, false, 1, false);
		CreateEdge(2,  true, 0,  true, 1, false);
		CreateEdge(2, false, 1,  true, 0, false);
		CreateEdge(2,  true, 1,  true, 0,  true);
	}

	// Corners
	if (true)
	{
		CreateCorner(0, false, 1, false, 2, false); // Right Top Front
		CreateCorner(1, false, 0, false, 2,  true); // Right Top Back
		CreateCorner(0, false, 1,  true, 2,  true); // Right Bottom Back
		CreateCorner(2, false, 1,  true, 0, false); // Right Bottom Front
		CreateCorner(0,  true, 1, false, 2,  true); // Left Top Back
		CreateCorner(0,  true, 2, false, 1, false); // Left Top Front
		CreateCorner(0,  true, 1,  true, 2, false); // Left Bottom Front
		CreateCorner(0,  true, 2,  true, 1,  true); // Left Bottom Back
	}

	// Connections to neighbors
	if (true)
	{
		if (forward) CreateArm(2, false, 0, false, 1, false);
		if (right) CreateArm(0, false, 1, false, 2, false);
		if (top) CreateArm(1, false, 2, false, 0, false);
		if (back) CreateArm(2,  true, 0, false, 1,  true);
		if (left) CreateArm(0,  true, 1, false, 2,  true);
		if (bottom) CreateArm(1,  true, 2, false, 0,  true);
	}

	ErrCheck("Chamber::Create\n");

	PostCreate();
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