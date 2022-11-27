#pragma once

#include "util.hpp" // Model class

enum class SideType
{
    closed = 0,
    tunnel = 1,
    chamber = 1, // TODO set to 2 when ready to handle chambers
};

class Structure : public Model
{
public:
    SideType sideType;
    
    // Whether there's a Structure in the neighboring position
	bool   right = true; // +X
	bool    left = true; // -X
	bool     top = true; // +Y
	bool  bottom = true; // -Y
	bool forward = true; // +Z
	bool    back = true; // -Z
protected:
    std::vector<std::vector<Vector3>> baseCenterlines;
    std::vector<std::vector<Vector3>> centerlines;

    void UnpackSides(unsigned char sides);
    void PostCreate();
    void DrawCenterlines();
};