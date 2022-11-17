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
	normals.push_back({0, 1, 0});
	vertices.push_back({0, 1, 0});
	for (float phi = 90 - d; phi >= 0; phi -= d)
	{
		Polar2Cart(1, phi, &r, &y);
		for (int theta = 0; theta <= 90; theta += d)
		{
			Polar2Cart(r, theta, &z, &x); // Flip X/Z to get CCW winding
			normals.push_back({x, y, z});
			vertices.push_back({x, y, z});
		}
	}

	// Assign indices for first row, all using the pole
	for (int col = 0; col < rowLength - 1; col++)
	{
		indices.push_back(poleIdx);
		indices.push_back(poleIdx + 1 + col);
		indices.push_back(poleIdx + 2 + col);
		indices.push_back(poleIdx);
	}

	// Assign indices for the rest of the rows
	int thisRowStart, nextRowStart;
	for (int row = 0; row < numRows - 1; row++)
	{
		thisRowStart = poleIdx+1 + rowLength*row;
		nextRowStart = thisRowStart + rowLength;
		for (int col = 0; col < rowLength - 1; col++)
		{
			indices.push_back(thisRowStart + 0 + col); // Top left
			indices.push_back(nextRowStart + 0 + col); // Bottom left
			indices.push_back(nextRowStart + 1 + col); // Bottom right
			indices.push_back(thisRowStart + 1 + col); // Top right
		}
	}
}
void Corner::CreateOneTunnel()
{
	float y, z;
	for (int theta=0; theta <= 90; theta += (360 / n))
	{
		Polar2Cart(1, theta, &y, &z);

		indices.push_back(vertices.size());
		normals.push_back({0, y, z});
		// glTexCoord2f(M_PI * theta / 360.0f, 0.0f);
		vertices.push_back({1, y, z});

		indices.push_back(vertices.size());
		normals.push_back({0, y, z});
		// glTexCoord2f(M_PI * theta / 360.0f, 1.0f);
		vertices.push_back({0, y, z});
	}
}
void Corner::CreateTwoTunnel()
{
	CreateOneTunnel();
}
void Corner::CreateThreeTunnel()
{
	CreateOneTunnel();
}

void Corner::Create()
{
	PreCreate();

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

	PostCreate();
}

void DrawClosed(
	const std::vector<std::vector<float>> &vertices,
	const std::vector<std::vector<float>> &normals,
	const std::vector<int> &indices,
	const float radius)
{
	glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
		// Convert vector of vectors to flat array
		float vertexArray[vertices.size() * 3];
		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			vertexArray[i*3 + 0] = vertices[i][0];
			vertexArray[i*3 + 1] = vertices[i][1];
			vertexArray[i*3 + 2] = vertices[i][2];
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
		}

		glPushMatrix(); {
			// FIXME radius doubled just for visibility
			glScalef(radius*2, radius*2, radius*2);

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray); 
			glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_BYTE, indexArray);
		} glPopMatrix();
	} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
}
void DrawOneTunnel(
	const std::vector<std::vector<float>> &vertices,
	const std::vector<std::vector<float>> &normals,
	const std::vector<int> &indices,
	const float radius)
{
	glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); {
		// Convert vector of vectors to flat array
		float vertexArray[vertices.size() * 3];
		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			vertexArray[i*3 + 0] = vertices[i][0];
			vertexArray[i*3 + 1] = vertices[i][1];
			vertexArray[i*3 + 2] = vertices[i][2];
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
		}

		glPushMatrix(); {
			// FIXME radius doubled just for visibility
			glScalef(0.5, radius*2, radius*2);

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray); 
			glDrawElements(GL_QUAD_STRIP, indices.size(), GL_UNSIGNED_BYTE, indexArray);
		} glPopMatrix();
	} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY);
}
void DrawTwoTunnels(
	const std::vector<std::vector<float>> &vertices,
	const std::vector<std::vector<float>> &normals,
	const std::vector<int> &indices,
	const float radius)
{
	// FIXME
	DrawOneTunnel(vertices, normals, indices, radius);
	glPushMatrix(); glRotatef(90, Y_AXIS); glRotatef(90, Z_AXIS); DrawOneTunnel(vertices, normals, indices, radius); glPopMatrix();
}
void DrawThreeTunnels(
	const std::vector<std::vector<float>> &vertices,
	const std::vector<std::vector<float>> &normals,
	const std::vector<int> &indices,
	const float radius)
{
	// FIXME
	DrawOneTunnel(vertices, normals, indices, radius);
	glPushMatrix(); glRotatef(90, Y_AXIS); glRotatef(90, Z_AXIS); DrawOneTunnel(vertices, normals, indices, radius); glPopMatrix();
	glPushMatrix(); glRotatef(-90, Z_AXIS); glRotatef(-90, Y_AXIS); DrawOneTunnel(vertices, normals, indices, radius); glPopMatrix();
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

	glPushMatrix(); {
		glScalef(baseScale[0]*meshScale[0], baseScale[1]*meshScale[1], baseScale[2]*meshScale[2]);
		glRotatef(meshRotate[0], X_AXIS);
		glRotatef(meshRotate[1], Y_AXIS);
		glRotatef(meshRotate[2], Z_AXIS);

		// TODO need a second key to handle diagonals eventually
		switch(surroundings.sqrMagnitude())
		{
		case 0:
			DrawClosed(vertices, normals, indices, radius);
			break;
		case 1:
			DrawOneTunnel(vertices, normals, indices, radius);
			break;
		case 2:
			DrawTwoTunnels(vertices, normals, indices, radius);
			break;
		case 3:
			DrawThreeTunnels(vertices, normals, indices, radius);
			break;
		// TODO handle chambers, too
		default:
			Fatal(999, "Unknown connection %d\n", surroundings.sqrMagnitude());
		}
	} glPopMatrix();
}

void Corner::Draw()
{
	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

		glDisable(GL_TEXTURE_2D);
        glColor4f(0.75 + baseScale[0]*.25, 0.75 + baseScale[1]*.25, 0.75 + baseScale[2]*.25, 0.5);
		glFrontFace(windMode);

        // FIXME shouldn't recalculate every time
        UpdateConnections();

		glFrontFace(GL_CCW);
		glEnable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 1);
    } glPopMatrix();
}
