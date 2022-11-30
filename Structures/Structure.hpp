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
    // {vertex index within this Structure, index of neighboring Structure within the parent}
	int   endpointRight[2] = {-1, -1}; // +X
	int    endpointLeft[2] = {-1, -1}; // -X
	int     endpointTop[2] = {-1, -1}; // +Y
	int  endpointBottom[2] = {-1, -1}; // -Y
	int endpointForward[2] = {-1, -1}; // +Z
	int    endpointBack[2] = {-1, -1}; // -Z
    int* GetEndpoint(int EPidx)
    {
        switch (EPidx)
        {
        case 0:
            return endpointRight;
        case 1:
            return endpointLeft;
        case 2:
            return endpointTop;
        case 3:
            return endpointBottom;
        case 4:
            return endpointForward;
        case 5:
            return endpointBack;
        default:
            Fatal(999, "Invalid endpoint index %d\n", EPidx);
            return NULL;
        }
    }

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