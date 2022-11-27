#include "Mine.hpp"
#include "globals.hpp"

Mine::Mine(unsigned char sides)
{
	type = 5;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	// Padding between edge of chamber and edge of cell
	float padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	UnpackSides(sides);

	Create();
}

void Mine::CreateInternal(int i0, bool f0, int i1, bool f1, int i2, bool f2)
{
    // Make arms going across in all directions (even if no exits at the ends)

	float r = 0.5 * padScale;
	float xAtTunnel = sqrt(r*r - tunnelRadius*tunnelRadius);

    // Maintain the same ratio as external panels
    int internalPanels;
    {
        float d = padding / armPanels;
        internalPanels = round((xAtTunnel - tunnelRadius) / d);
    }
	float d = (xAtTunnel - tunnelRadius) / internalPanels;
	float thetaD = 360 / tunnelN;

	Vector3 coords;
	float x = tunnelRadius, y, z;
	for (int i = 0; i < internalPanels; i++)
	{
		for (float theta = 0; theta < 360; theta += thetaD)
		{
			// Top-left quadrant, moving CCW
			{ // This row
				Polar2Cart(tunnelRadius, theta, &z, &y);
				z = -z;
				coords[i1] = f1 ? -y : y;
				coords[i2] = f2 ? -z : z;

				// Outer edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);

				// Inner edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
                // FIXME generalize to n != 8
                if (i == 0)
                    coords[i0] = (f0 ? -1 : 1) * ((abs(y) >= abs(z)) ? abs(y) : abs(z));
                else
				    coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);
			}
			{ // Next row
				Polar2Cart(tunnelRadius, theta + thetaD, &z, &y);
				z = -z;
				coords[i1] = f1 ? -y : y;
				coords[i2] = f2 ? -z : z;

				// Inner edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
                // FIXME generalize to n != 8
                if (i == 0)
                    coords[i0] = (f0 ? -1 : 1) * ((abs(y) >= abs(z)) ? abs(y) : abs(z));
                else
				    coords[i0] = f0 ? -x : x;
				vertices.push_back(coords);

				// Outer edge
				coords[i0] = 0;
				indices.push_back(vertices.size()); quadIndices.push_back(vertices.size());
				normals.push_back(coords);
				coords[i0] = (f0 ? -1 : 1) * (x + d);
				vertices.push_back(coords);
			}
		}

		x += d;
	}
}
void Mine::Create()
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

    // Internals
    if (true)
    {
		CreateInternal(0, false, 1, false, 2, false); // right
		CreateInternal(1, false, 2, false, 0, false); // top
		CreateInternal(2, false, 0, false, 1, false); // forward
		CreateInternal(0,  true, 1, false, 2,  true); // left
		CreateInternal(1,  true, 2, false, 0,  true); // bottom
		CreateInternal(2,  true, 0, false, 1,  true); // back
    }

	ErrCheck("Chamber::Create\n");

	PostCreate();
}