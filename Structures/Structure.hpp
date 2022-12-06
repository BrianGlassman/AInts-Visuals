#pragma once

#include "Model.hpp"
#include "Vertex.hpp"
#include "Vector.hpp"

enum class SideType
{
    closed = 0,
    tunnel = 1,
    chamber = 1, // CHAMBERCONNECT set to 2 when ready to handle chambers // NORELEASE
    hill = 2, // Gives a way to only connect to the bottom of hills
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
    void SetSide(const Vector3Int direction, bool newVal)
    {
        auto hash = std::hash<Vector3Int>()(direction);
        switch(hash)
        {
            case Vector3Int::hashRight:
                right = newVal;
                break;
            case Vector3Int::hashLeft:
                left = newVal;
                break;
            case Vector3Int::hashUp:
                top = newVal;
                break;
            case Vector3Int::hashDown:
                bottom = newVal;
                break;
            case Vector3Int::hashForward:
                forward = newVal;
                break;
            case Vector3Int::hashBackward:
                back = newVal;
                break;
            default:
                Fatal(999, "Invalid side direction hash %d (%d, %d, %d)\n", hash, direction.x, direction.y, direction.z);
        }
    }

    // Index of the vertex that is the endpoint in that direction
    // {vertex index within this Structure, index of neighboring Structure within the parent}
	int   endpointRight = -1; // +X
	int    endpointLeft = -1; // -X
	int     endpointTop = -1; // +Y
	int  endpointBottom = -1; // -Y
	int endpointForward = -1; // +Z
	int    endpointBack = -1; // -Z
    int GetEndpoint(int EPidx)
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
            return -1;
        }
    }
    int GetEndpoint(const Vector3Int direction)
    {
        auto hash = std::hash<Vector3Int>()(direction);
        switch(hash)
        {
            case Vector3Int::hashRight:
                return endpointRight;
            case Vector3Int::hashLeft:
                return endpointLeft;
            case Vector3Int::hashUp:
                return endpointTop;
            case Vector3Int::hashDown:
                return endpointBottom;
            case Vector3Int::hashForward:
                return endpointForward;
            case Vector3Int::hashBackward:
                return endpointBack;
            default:
                Fatal(999, "Invalid endpoint direction hash %d (%d, %d, %d)\n", hash, direction.x, direction.y, direction.z);
                return -1;
        }
    }

    virtual void ApplyNoise();

    std::vector<Vertex>* getPerturbedCL();
    std::vector<Vertex>* getBaseCL();
    std::vector<Vertex>* getCL();
protected:
    std::vector<Vertex> baseCenterline;
    std::vector<Vertex> centerline;

    void PreCreate();
    void ApplyNoiseCLHelper(unsigned int i, float scale);
    void DrawCLHelper(GLenum mode, std::vector<Vertex> CLtoUse);
    void DrawCenterlines();
};