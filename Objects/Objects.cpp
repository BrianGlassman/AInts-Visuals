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
    std::shared_ptr<OBJ> MineCore;
    std::shared_ptr<OBJ> MineRightCap;
    std::shared_ptr<OBJ> MineLeftCap;
    std::shared_ptr<OBJ> MineTopCap;
    std::shared_ptr<OBJ> MineBottomCap;
    std::shared_ptr<OBJ> MineFrontCap;
    std::shared_ptr<OBJ> MineBackCap;
    // OBJ Farm;
}

void InitOBJs()
{
    Vector3 ore({256/256.0, 200/256.0, 150/256.0}); // Rust-colored iron ore
    Vector3 outer({1, 1, 1}); // Unchanged texture
    Vector3 inner({0.4, 0.4, 0.5}); // Greyer, rockier soil

    float mineScale = 115;

    Objects::MineCore = std::make_shared<OBJ>("Mine4-0.obj", mineScale);
    Objects::MineCore->OBJcolors = {ore, outer, inner};
    Objects::MineRightCap = std::make_shared<OBJ>("Mine4-1.obj", mineScale);;
    Objects::MineRightCap->OBJcolors = {outer};
    Objects::MineLeftCap = std::make_shared<OBJ>("Mine4-6.obj", mineScale);
    Objects::MineLeftCap->OBJcolors = {ore, outer, inner};
    Objects::MineTopCap = std::make_shared<OBJ>("Mine4-2.obj", mineScale);
    Objects::MineTopCap->OBJcolors = {ore, outer, inner};
    Objects::MineBottomCap = std::make_shared<OBJ>("Mine4-3.obj", mineScale);
    Objects::MineBottomCap->OBJcolors = {outer, inner};
    Objects::MineFrontCap = std::make_shared<OBJ>("Mine4-4.obj", mineScale);
    Objects::MineFrontCap->OBJcolors = {outer, inner};
    Objects::MineBackCap = std::make_shared<OBJ>("Mine4-5.obj", mineScale);
    Objects::MineBackCap->OBJcolors = {ore, outer, inner};
    // Something is very wrong though
    // Still drawing the outer sphere, and also the cube walls aren't getting changed by the scaling factor
}