#pragma once

#include <unordered_map>
#include <vector>

#include "CSCIx229.h"
#include "Vector.hpp"

// Used in various places, so not all elements are used in each
enum class StructureType
{
    Delete = 0,
    Model = 1,
    Tunnel = 2,
    Chamber = 3,
    Colony = 4,
    Mine = 5,
    Hill = 6,
    Farm = 7,
    MinePiece = 8,
    NONE = 99,
};

//---------
// Toggles
//---------
namespace Toggles
{
    namespace Light
    {
        extern bool lightOrbiting;
        extern bool smooth;
    }
    namespace Noise
    {
        extern bool showPerturbed;
        extern bool showPVectors;
        extern bool showBase;
    }
    namespace Display
    {
        extern bool showAxes;
        extern bool wireframe;
        extern bool faceCulling;
    }

    extern bool showNormals;

    extern bool debug;

    extern bool showCenterlines;

    extern bool printMove;
}

//--------
// Values
//--------
// Enum that can be incremented or decremented, adapted from https://stackoverflow.com/a/15451002
enum class SetColorMode : unsigned { START_OF_LIST, glColor, materialProps, both, END_OF_LIST };
SetColorMode& operator++(SetColorMode &val );
SetColorMode operator++(SetColorMode &val, int );
SetColorMode& operator--(SetColorMode &val );
SetColorMode operator--(SetColorMode &val, int );

namespace Globals
{
    extern float tunnelNoiseScale;
    extern float chamberNoiseScale;
    extern StructureType toBuild;
	extern SetColorMode setColorMode;
}

//-------------------
// Input --> Display
//-------------------
namespace Globals
{
    extern float mouse_x;
    extern float mouse_y;
    extern float mouse_zoom;
}

//-------------------------
// Main, Input --> Display
//-------------------------
// Enum that can be incremented or decremented, adapted from https://stackoverflow.com/a/15451002
enum class Scene : unsigned { START_OF_LIST, colony, tunnel, chamber, mine, farm, 
    hill, allTunnels, END_OF_LIST };
Scene& operator++(Scene &val );
Scene operator++(Scene &val, int );
Scene& operator--(Scene &val );
Scene operator--(Scene &val, int );
namespace Globals
{
    extern Scene sceneChoice;
}

extern float baseMag;

//------------------------
// Main, Input --> Window
//------------------------
extern float baseMag;
extern int windowWidth;
extern int windowHeight;

//--------------------
// Dispaly --> Window
//--------------------
enum class ViewMode { INTERIOR, EXTERIOR };
class Colony; // Forward declaration for _InteriorView
struct _InteriorView{
    Colony* pathHolder;
    int pathIdx;
    Vector3 up;
    Vector3 right;
    float rotH;
    float rotV;
    Vector3 eyePos;
    Vector3 lookDir;

    _InteriorView()
    {
        pathHolder = nullptr;
        pathIdx = 0;
        up = {0, 1, 0};
        rotH = 180;
        rotV = 0;
        eyePos = {1, -1, 0};
        lookDir = {
             (float)(Sin(rotH)*Cos(rotV)),
            -(float)(Sin(rotV)),
            -(float)(Cos(rotH)*Cos(rotV))};
    }
};
namespace Globals
{
    extern ViewMode viewMode;
    extern std::unordered_map<Scene, _InteriorView> IVs;
    extern _InteriorView* InteriorView;
}