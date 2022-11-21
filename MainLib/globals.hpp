#pragma once

//---------
// Toggles
//---------
namespace Toggles
{
    extern bool lightOrbiting;
    namespace Noise
    {
        extern bool showPVectors;
    }
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
extern Scene sceneChoice;

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
        extern float up[3];
        extern float rotH, rotV;
        extern float eyePos[3];
        extern float lookDir[3];
    }
}