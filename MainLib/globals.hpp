#pragma once

//-------------------
// Input --> Display
//-------------------
extern float mouse_x;
extern float mouse_y;
extern float mouse_zoom;

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