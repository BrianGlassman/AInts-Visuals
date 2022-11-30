#pragma once

#include "util.hpp" // Model class
#include "Vertex.hpp"

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

    // Index of the vertex that is the endpoint in that direction
	int   endpointRight = -1; // +X
	int    endpointLeft = -1; // -X
	int     endpointTop = -1; // +Y
	int  endpointBottom = -1; // -Y
	int endpointForward = -1; // +Z
	int    endpointBack = -1; // -Z

    std::vector<Vertex>* getPerturbedCL();
    std::vector<Vertex>* getBaseCL();
    std::vector<Vertex>* getCL();
protected:
    std::vector<Vertex> baseCenterline;
    std::vector<Vertex> centerline;

    void UnpackSides(unsigned char sides);
    void PreCreate();
    void DrawCLHelper(GLenum mode, std::vector<Vertex> CLtoUse);
    void DrawCenterlines();
};