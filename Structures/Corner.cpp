#include "Corner.hpp"
#include "Structure.hpp"

Corner::Corner()
{
}

void Corner::SetRotateAndScale()
{
	// Base meshes are defined with X >= Y >= Z 
	// So rotate to match that convention

	if (surroundings.x >= surroundings.y && surroundings.x >= surroundings.z)
	{
		if (surroundings.y >= surroundings.z)
		{
			// x >= y >= z - Matches default
			meshRotate[0] = 0; meshRotate[1] = 0; meshRotate[2] = 0;
			meshScale[0] = 1; meshScale[1] = 1; meshScale[2] = 1;
			windCW = false;
		}
		else
		{
			// x >= z >= y - Switch Y and Z
			meshRotate[0] = 90; meshRotate[1] = 0; meshRotate[2] = 0;
			meshScale[0] = 1; meshScale[1] = -1; meshScale[2] = 1;
			windCW = true;
		}
	}
	else if (surroundings.y >= surroundings.x && surroundings.y >= surroundings.z)
	{
		if (surroundings.z >= surroundings.x)
		{
			// y >= z >= x - X --> Z, Y --> X, Z --> Y
			meshRotate[0] = 0; meshRotate[1] = 90; meshRotate[2] = 90;
			meshScale[0] = 1; meshScale[1] = 1; meshScale[2] = 1;
			windCW = false;
		}
		else
		{
			// y >= x >= z - Switch X and Y
			// NOTE: only used once chambers matter
			// meshRotate[0] = 0; meshRotate[1] =  0; meshRotate[2] = 90;
			// meshScale[0] = -1; meshScale[1] = 1; meshScale[2] = 1;
			// windCW = false;
		}
	}
	else if (surroundings.z >= surroundings.x && surroundings.z >= surroundings.y)
	{
		if (surroundings.x >= surroundings.y)
		{
			// z >= x >= y - X --> Y, Y --> Z, Z --> X
			meshRotate[0] = -90; meshRotate[1] = 0; meshRotate[2] = -90;
			meshScale[0] = 1; meshScale[1] = 1; meshScale[2] = 1;
			windCW = false;
		}
		else
		{
			// z >= y >= x
			// NOTE: only used once chambers matter
			// meshRotate[0] = 0; meshRotate[1] = -90; meshRotate[2] = 0;
			// meshScale[0] = -1; meshScale[1] = 1; meshScale[2] = 1;
			// windCW = false;
		}
	}
	else
	{
		Fatal(999, "SetRotateAndScale fell through");
	}

	// Mirroring reverses the face culling behavior, so have to correct for that
	if (baseScale[0]*baseScale[1]*baseScale[2] < 0)
	{
		windCW = not windCW;
	}

	if (windCW)
		windMode = GL_CW;
	else
		windMode = GL_CCW;
}

SideType GetNodeType(int x, int y, int z)
{
	// FIXME need to get the neighbor
	Structure structure;

	return structure.sideType;
}

void Corner::CreateClosed()
{
	int d = 360 / n;
	float r, y;
	float x, z;

	// Quarter sphere, using degenerate quads for the caps
	int rowLength = n / 4 + 1;
	int numRows = rowLength - 1; // Pole doesn't count as a row
	int poleIdx = vertices.size();

	// Generate the vertices, starting from the pole
	normals.push_back({0, radius, 0});
	vertices.push_back({0, radius, 0});
	for (float phi = 90 - d; phi >= 0; phi -= d)
	{
		Polar2Cart(radius, phi, &r, &y);
		for (int theta = 0; theta <= 90; theta += d)
		{
			Polar2Cart(r, theta, &z, &x); // Flip X/Z to get CCW winding
			normals.push_back({x, y, z});
			vertices.push_back({x, y, z});
		}
	}

	// Assign indices for first row, all using the pole
	for (int col = 0; col < rowLength; col++)
	{
		indices.push_back(poleIdx);
		indices.push_back(poleIdx + 1 + col);
	}
	indexBounds.push_back(indices.size());

	// Assign indices for the rest of the rows
	int thisRowStart, nextRowStart;
	for (int row = 0; row < numRows - 1; row++)
	{
		thisRowStart = poleIdx+1 + rowLength*row;
		nextRowStart = thisRowStart + rowLength;
		for (int col = 0; col < rowLength; col++)
		{
			indices.push_back(thisRowStart + col);
			indices.push_back(nextRowStart + col);
		}
		indexBounds.push_back(indices.size());
	}
}
void Corner::CreateOneTunnel()
{
	// Tunnel along "x" - stop at plane X=0
	float x, y, z;
	for (int theta=0; theta <= 90; theta += (360 / n))
	{
		Polar2Cart(radius, theta, &y, &z);
		x = 0;

		// Outer point
		indices.push_back(vertices.size());
		normals.push_back({x, y, z});
		// glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
		vertices.push_back({0.5, y, z});

		// Inner point
		indices.push_back(vertices.size());
		normals.push_back({0, y, z});
		// glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
		vertices.push_back({x, y, z});
	}
}
void Corner::CreateTwoTunnel()
{
	std::vector<int> yInner;

	// Tunnel along X - stop at plane X=Y
	{
		float x, y, z;
		for (int theta=0; theta <= 90; theta += (360 / n))
		{
			Polar2Cart(radius, theta, &y, &z);
			x = y;

			// Outer point
			indices.push_back(vertices.size());
			normals.push_back({0, y, z});
			// glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
			vertices.push_back({0.5, y, z});

			// Inner point
			indices.push_back(vertices.size());
			yInner.push_back(vertices.size());
			normals.push_back({0, y, z});
			// glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
			vertices.push_back({x, y, z});
		}
	}

	indexBounds.push_back(indices.size());

	// Tunnel along Y - stop at plane X=Y, re-use inner points
	{
		int yInnerIdx = 0;
		float x, z;
		for (int theta=0; theta <= 90; theta += (360 / n))
		{
			Polar2Cart(radius, theta, &x, &z);

			// Re-use inner point
			indices.push_back(yInner[yInnerIdx]);
			yInnerIdx++;

			// Outer point
			indices.push_back(vertices.size());
			normals.push_back({x, 0, z});
			// FIXME texture
			vertices.push_back({x, 0.5, z});
		}
	}
}
void Corner::CreateThreeTunnel()
{
	std::vector<int> yInner;
	std::vector<int> zInner;

	// Tunnel along X - stop at planes X=Y or X=Z
	{
		float x, y, z;
		for (int theta=90; theta >= 0; theta -= (360 / n))
		{
			Polar2Cart(radius, theta, &y, &z);

			// Inner point
			indices.push_back(vertices.size());
			normals.push_back({0, y, z});
			// glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
			if (y > z)
			{
				x = y;
				yInner.push_back(vertices.size());
				// fprintf(stdout, "add to yInner (%f, %f, %f)\n", x, y, z);
			}
			else if (y == z)
			{
				x = y;
				yInner.push_back(vertices.size());
				// fprintf(stdout, "add to yInner (%f, %f, %f)\n", x, y, z);
				zInner.push_back(vertices.size());
			}
			else
			{
				x = z;
				zInner.push_back(vertices.size());
			}
			vertices.push_back({x, y, z});

			// Outer point
			indices.push_back(vertices.size());
			normals.push_back({0, y, z});
			// glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
			vertices.push_back({0.5, y, z});
		}
	}

	indexBounds.push_back(indices.size());

	// Tunnel along Y - stop at planes Y=X or Y=Z, re-use inner points for Y=X
	{
		int yInnerIdx = 0;
		float x, z;
		for (int theta=90; theta >= 0; theta -= (360 / n))
		{
			Polar2Cart(radius, theta, &x, &z);

			// Outer point
			indices.push_back(vertices.size());
			normals.push_back({x, 0, z});
			// FIXME texture
			vertices.push_back({x, 0.5, z});

			// Inner point
			if (x >= z)
			{
				// Re-use from X=Y
				indices.push_back(yInner[yInnerIdx]);
				// fprintf(stdout, "reu %f, %f, %f\n", vertices[yInner[yInnerIdx]][0], vertices[yInner[yInnerIdx]][1], vertices[yInner[yInnerIdx]][2]);
				yInnerIdx++;
			}
			else
			{
				// Create a new point on Y=Z
				float y = z;
				indices.push_back(vertices.size());
				zInner.push_back(vertices.size());
				normals.push_back({x, 0, z});
				// FIXME texture
				vertices.push_back({x, y, z});
				// fprintf(stdout, "new %f, %f, %f\n", x, y, z);
			}
		}
	}

	indexBounds.push_back(indices.size());

	// for (int i = 0; i < yInner.size(); i++)
	// {
	// 	fprintf(stdout, "%d (%f, %f, %f), ", yInner[i], vertices[yInner[i]][0], vertices[yInner[i]][1], vertices[yInner[i]][2]);
	// }
	// fprintf(stdout, "\n");

	// Tunnel along Z - re-use inner points
	{
		int zInnerIdx = 0;
		float x, y;
		for (int theta=0; theta <= 90; theta += (360 / n))
		{
			Polar2Cart(radius, theta, &x, &y);

			// Outer point
			indices.push_back(vertices.size());
			normals.push_back({x, y, 0});
			// FIXME texture
			vertices.push_back({x, y, 0.5});

			// Inner point, re-use from Z=X or Z=Y
			indices.push_back(zInner[zInnerIdx]);
			zInnerIdx++;
		}
	}
}

void Corner::Create()
{
	PreCreate();
	indexBounds.clear();
	indexBounds.push_back(0);

    UpdateConnections();

	// TODO need a second key to handle diagonals eventually
	switch(surroundings.sqrMagnitude())
	{
	case 0:
		CreateClosed();
		break;
	case 1:
		CreateOneTunnel();
		break;
	case 2:
		CreateTwoTunnel();
		break;
	case 3:
		CreateThreeTunnel();
		break;
	// TODO handle chambers, too
	default:
		Fatal(999, "Unknown connection %d\n", surroundings.sqrMagnitude());
	}

	// Apply scaling and rotation to the created vertices and normals
	for (auto&& vertex : vertices)
	{
		float x = vertex[0], y = vertex[1], z = vertex[2];

		// fprintf(stdout, "Pre (%f, %f, %f) rot (%f, %f, %f) * (%f, %f, %f)\n", 
		// 	vertex[0], vertex[1], vertex[2],
		// 	meshRotate[0], meshRotate[1], meshRotate[2],
		// 	meshScale[0], meshScale[1], meshScale[2]);

		// Rotate around Z axis
		vertex[0] =  Cos(meshRotate[2])*x - Sin(meshRotate[2])*y;
		vertex[1] =  Sin(meshRotate[2])*x + Cos(meshRotate[2])*y;
		vertex[2] =  z;
		x = vertex[0]; y = vertex[1]; z = vertex[2];

		// fprintf(stdout, "Post Z (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]);

		// Rotate around Y axis
		vertex[0] =  Cos(meshRotate[1])*x + Sin(meshRotate[1])*z;
		vertex[1] =  y;
		vertex[2] = -Sin(meshRotate[1])*x + Cos(meshRotate[1])*z;
		x = vertex[0]; y = vertex[1]; z = vertex[2];

		// fprintf(stdout, "Post Y (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]);

		// Rotate around X axis
		vertex[0] =  x;
		vertex[1] =  Cos(meshRotate[0])*y - Sin(meshRotate[0])*z;
		vertex[2] =  Sin(meshRotate[0])*y + Cos(meshRotate[0])*z;
		x = vertex[0]; y = vertex[1]; z = vertex[2];

		// fprintf(stdout, "Post X (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]);

		// Scale
		vertex[0] *= baseScale[0] * meshScale[0];
		vertex[1] *= baseScale[1] * meshScale[1];
		vertex[2] *= baseScale[2] * meshScale[2];

		// fprintf(stdout, "Post scale (%f, %f, %f)\n", vertex[0], vertex[1], vertex[2]);
	}

	indexBounds.push_back(indices.size());
	PostCreate();
}

void Corner::UpdateConnections()
{
	//Surroundings surroundings = {0,0,0, 0,0,0};
    /*
	// Handle adjacent nodes
	surroundings.x = (int)GetNodeType(1, 0, 0);
	surroundings.y = (int)GetNodeType(0, 1, 0);
	surroundings.z = (int)GetNodeType(0, 0, 1);
	// Handle diagonal nodes
	surroundings.xy = (surroundings.x == 2 && surroundings.y == 2) ? (int)GetNodeType(1, 1, 0) : 0;
	surroundings.xz = (surroundings.x == 2 && surroundings.y == 2) ? (int)GetNodeType(1, 0, 1) : 0;
	surroundings.yz = (surroundings.x == 2 && surroundings.y == 2) ? (int)GetNodeType(0, 1, 1) : 0;
    */
	
	//int key[2] = {surroundings.sqrMagnitude(), 0};

	SetRotateAndScale();
}

void Corner::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		glDisable(GL_TEXTURE_2D);
        glColor4f(0.75 + baseScale[0]*.25, 0.75 + baseScale[1]*.25, 0.75 + baseScale[2]*.25, 0.5);
		glFrontFace(windMode);

		// FIXME array-ification can happen in Create, doesn't need to be in Draw
		glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
			// Convert vector of vectors to flat array
			float vertexArray[vertices.size() * 3];
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				vertexArray[i*3 + 0] = vertices[i][0];
				vertexArray[i*3 + 1] = vertices[i][1];
				vertexArray[i*3 + 2] = vertices[i][2];
				// fprintf(stdout, "%d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]);
			}
			float normalArray[normals.size() * 3];
			for (unsigned int i = 0; i < normals.size(); i++)
			{
				normalArray[i*3 + 0] = normals[i][0];
				normalArray[i*3 + 1] = normals[i][1];
				normalArray[i*3 + 2] = normals[i][2];
			}

			unsigned char indexArray[indices.size()];
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				indexArray[i] = indices[i];
				// fprintf(stdout, "%d\n", indices[i]);
			}

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			for (unsigned int i = 0; i < indexBounds.size() - 1; i++)
			{
				int count = indexBounds[i+1] - indexBounds[i];
				glDrawElements(GL_QUAD_STRIP, count, GL_UNSIGNED_BYTE, indexArray + indexBounds[i] * sizeof(unsigned char));
			}
		} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);

		glFrontFace(GL_CCW);
		glEnable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 1);
    } glPopMatrix();
}
