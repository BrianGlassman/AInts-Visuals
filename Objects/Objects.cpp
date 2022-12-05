#include <iostream>
#include <fstream>

#include "Objects.hpp"
#include "globals.hpp"

// File reading adapted from https://cplusplus.com/doc/tutorial/files/

std::string dir = "Objects/";
float MVscale = 10; // MagicaVoxel uses a 1:10 scale

Vector3 ReadVertex(const std::string line)
{
    Vector3 vec;

    // Parse "v x y z"
    std::size_t next = 1;
    std::size_t inc;
    for (auto&& i = 0; i < 3; i++)
    {
        vec[i] = std::stof(line.substr(next), &inc);
        next += inc + 1;
    }
    // printf("vertex %f, %f, %f\n", vec.x, vec.y, vec.z); // NORELEASE

    return vec;
}

Vector3 ReadNormal(const std::string line)
{
    Vector3 vec;

    // Parse "vn x y z"
    std::size_t next = 2;
    std::size_t inc;
    for (auto&& i = 0; i < 3; i++)
    {
        vec[i] = std::stof(line.substr(next), &inc);
        next += inc + 1;
    }
    // printf("normal %f, %f, %f\n", vec.x, vec.y, vec.z); // NORELEASE

    return vec;
}

Vector3 ReadTXCoord(const std::string line)
{
    Vector3 vec;

    // Parse "vt x y"
    std::size_t next = 2;
    std::size_t inc;
    for (auto&& i = 0; i < 2; i++)
    {
        vec[i] = std::stof(line.substr(next), &inc);
        next += inc + 1;
    }
    // printf("texture coord %f, %f\n", vec.x, vec.y); // NORELEASE

    return vec;
}

Objects::Face ReadFace(const std::string line)
{
    Objects::Face face;

    // Parse "f v/c/n v/c/n v/c/n"
    std::size_t next = 1;
    std::size_t inc;
    int v, c, n;
    for (auto&& i = 0; i < 3; i++)
    {
        // Note: OBJ format is 1-indexed, so have to subtract
        v = std::stoi(line.substr(next), &inc) - 1;
        next += inc + 1;
        c = std::stoi(line.substr(next), &inc) - 1;
        next += inc + 1;
        n = std::stoi(line.substr(next), &inc) - 1;
        next += inc + 1;
        // printf("v = %d, t = %d, n = %d\n", v, c, n); // NORELEASE
        face.vIdxs[i] = v; face.cIdxs[i] = c; face.nIdxs[i] = n;
    }

    return face;
}

OBJ::OBJ(std::string filename, float _scale)
{
    // Open file
    std::vector<std::string> lines;
    {
        std::string objPath = dir + filename;
        std::ifstream file(objPath);
        if (!file.is_open()) Fatal(999, "Failed to open file %d\n", objPath);
        std::string line;
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
    }

    for (auto& line : lines)
    {
        if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
            OBJnormals.push_back(ReadNormal(line));
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
            OBJcolors.push_back(ReadTXCoord(line));
        else if (line[0]=='v' && line[1]==' ')
            OBJvertices.push_back(ReadVertex(line));
        else if (line[0] == 'f' && line[1] == ' ')
            OBJfaces.push_back(ReadFace(line));
        // Can ignore "usemtl", always [filename].mtl
        // Can ignore "mtllib", always palette
        // else: skip
    }

    scale = _scale / MVscale; // FIXME scale doesn't get used yet
}

void OBJ::PreCreate()
{
    Structure::PreCreate();
    colors.clear();
}

void OBJ::Create()
{
    PreCreate();

    // Convert OBJ system to the system everything else uses
    for (auto& face : OBJfaces)
    {
        for (int i = 0; i < 3; i ++)
        {
            indices.push_back(vertices.size());
            vertices.push_back(OBJvertices[face.vIdxs[i]] * (1.0 / scale));
            normals.push_back(OBJnormals[face.nIdxs[i]]);
            colors.push_back(OBJcolors[face.cIdxs[i]]);
        }
    }

    PostCreate();
}

void OBJ::DrawHelper(std::vector<Vector3> drawVertices)
{
	// FIXME array-ification can happen in Create, doesn't need to be in Draw // NORELEASE
	glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_NORMAL_ARRAY); glEnableClientState(GL_COLOR_ARRAY); {
		// Convert vector of vectors to flat array
		float vertexArray[drawVertices.size() * 3];
		for (unsigned int i = 0; i < drawVertices.size(); i++)
		{
			vertexArray[i*3 + 0] = drawVertices[i][0];
			vertexArray[i*3 + 1] = drawVertices[i][1];
			vertexArray[i*3 + 2] = drawVertices[i][2];
			// fprintf(stdout, "vertexArray %d: (%f, %f, %f)\n", i, vertexArray[i*3 + 0], vertexArray[i*3 + 1], vertexArray[i*3 + 2]); // NORELEASE
		}
		float normalArray[normals.size() * 3];
		for (unsigned int i = 0; i < normals.size(); i++)
		{
			normalArray[i*3 + 0] = normals[i][0];
			normalArray[i*3 + 1] = normals[i][1];
			normalArray[i*3 + 2] = normals[i][2];
		}
        float colorArray[colors.size() * 3];
        for (unsigned int i = 0; i < colors.size(); i++)
        {
            colorArray[i*3 + 0] = colors[i][0];
            colorArray[i*3 + 1] = colors[i][1];
            colorArray[i*3 + 2] = colors[i][2];
        }

		if (indices.size() > 0)
		{ // Draw with GL_TRIANGLES
			unsigned int triIndexArray[indices.size()];
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				triIndexArray[i] = indices[i];
				// fprintf(stdout, "%d\n", triIndexArray[i]); // NORELEASE
			}
			// fprintf(stdout, "%d: %d\n", indices.size()-1, triIndexArray[triIndices.size()-1]); // Check for overflow // NORELEASE

			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
            glColorPointer(3, GL_FLOAT, 0, colorArray);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, triIndexArray);
		}
	} glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_COLOR_ARRAY);
}

void OBJ::Draw(bool hasControl)
{
    // Perturbed geometry
    if (Toggles::Noise::showPerturbed)
        DrawHelper(vertices);
    // Only show base if not showing perturbed, too cluttered to show both
    else if (Toggles::Noise::showBase)
        DrawHelper(baseVertices);
}

namespace Objects {
    std::shared_ptr<OBJ> Mine;
    // OBJ Farm;
}

void InitOBJs()
{
    // Use 140 instead of 128 to make sure it fits inside the Chamber walls
    Objects::Mine = std::make_shared<OBJ>("Mine.obj", 140);
    Objects::Mine->OBJcolors = std::vector<Vector3>({
        {256/256.0, 200/256.0, 150/256.0},
        {0.3, 0.3, 1},
        {1, 1, 1},
    });
}