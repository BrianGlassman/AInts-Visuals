#include <iostream>
#include <fstream>

#include "Objects.hpp"
#include "globals.hpp"

// File reading adapted from https://cplusplus.com/doc/tutorial/files/

std::string dir = "Objects/";
float MVscale = 10; // MagicaVoxel uses a 1:10 scale

Vector3 ReadVertex(const std::string line, float scale)
{
    Vector3 vec;

    // Parse "v x y z"
    std::size_t next = 1;
    std::size_t inc;
    for (auto&& i = 0; i < 3; i++)
    {
        vec[i] = std::stof(line.substr(next), &inc) / scale;
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

    // MagicaVoxel expects you to look at the outside, so reverse the normals
    return vec.Reversed();
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
    for (auto&& i = 0; i <= 2; i++)
    {
        // Note: OBJ format is 1-indexed, so have to subtract
        v = std::stoi(line.substr(next), &inc) - 1;
        next += inc + 1;
        c = std::stoi(line.substr(next), &inc) - 1;
        next += inc + 1;
        n = std::stoi(line.substr(next), &inc) - 1;
        next += inc + 1;
        // printf("v = %d, t = %d, n = %d\n", v, c, n); // NORELEASE

        // MagicaVoxel expects you to look at the outside, so reverse the winding
        face.vIdxs[2-i] = v; face.cIdxs[2-i] = c; face.nIdxs[2-i] = n;
    }

    return face;
}

OBJ::OBJ(std::string filename, float _scale)
{
    scale = _scale / MVscale;

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
            OBJvertices.push_back(ReadVertex(line, scale));
        else if (line[0] == 'f' && line[1] == ' ')
            OBJfaces.push_back(ReadFace(line));
        // Can ignore "usemtl", always [filename].mtl
        // Can ignore "mtllib", always palette
        // else: skip
    }
}

namespace Objects {
    std::shared_ptr<OBJ> Mine;
    // OBJ Farm;
}

void InitOBJs()
{
    // Use 144 (=128 + 16) instead of 128 to make sure it fits inside the Chamber walls
    Objects::Mine = std::make_shared<OBJ>("Mine.obj", 144);
    Objects::Mine->OBJcolors = std::vector<Vector3>({
        {256/256.0, 200/256.0, 150/256.0},
        {0.3, 0.3, 1},
        {1, 1, 1},
    });
}