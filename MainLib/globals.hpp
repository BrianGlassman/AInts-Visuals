#pragma once

#include "Vector.hpp"

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

    extern bool showNormals;

    extern bool debug;

    extern bool showCenterlines;

    extern bool printMove;
}

//--------
// Values
//--------
namespace Globals
{
    extern float tunnelNoiseScale;
    extern float chamberNoiseScale;
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
enum class Scene : unsigned { START_OF_LIST, colony, tunnel, chamber, allTunnels, END_OF_LIST };
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
namespace Globals
{
    extern ViewMode viewMode;
    namespace InteriorView {
        extern Vector3 up;
        extern Vector3 right;
        extern float rotH, rotV;
        extern Vector3 eyePos;
        extern Vector3 lookDir;
    }
}