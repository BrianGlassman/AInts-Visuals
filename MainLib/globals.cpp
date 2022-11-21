#include <type_traits>

#include "globals.hpp"

#include "CSCIx229.h"

//---------
// Toggles
//---------
bool Toggles::lightOrbiting = true;
bool Toggles::Noise::showPVectors = false;

//-------------------
// Input --> Display
//-------------------
float Globals::mouse_x = 0.5f;
float Globals::mouse_y = 0.5f;
float Globals::mouse_zoom = 1;

//-------------------------
// Main, Input --> Display
//-------------------------
// Special behavior for ++Scene
Scene& operator++(Scene &val ) {
  using IntType = typename std::underlying_type<Scene>::type;
  val = static_cast<Scene>( static_cast<IntType>(val) + 1 );
  if (val == Scene::END_OF_LIST )
    val = static_cast<Scene>( static_cast<IntType>(Scene::START_OF_LIST) + 1 );
  return val;
}
// Special behavior for Scene++
Scene operator++(Scene &val, int ) {
  Scene result = val;
  ++val;
  return result;
}
// Special behavior for --Scene
Scene& operator--(Scene &val ) {
  using IntType = typename std::underlying_type<Scene>::type;
  val = static_cast<Scene>( static_cast<IntType>(val) - 1 );
  if (val == Scene::START_OF_LIST )
    val = static_cast<Scene>( static_cast<IntType>(Scene::END_OF_LIST) - 1 );
  return val;
}
// Special behavior for Scene--
Scene operator--(Scene &val, int ) {
  Scene result = val;
  --val;
  return result;
}

Scene sceneChoice;

//------------------------
// Main, Input --> Window
//------------------------
float baseMag = 4;
int windowWidth = 900;
int windowHeight = 900;

//--------------------
// Dispaly --> Window
//--------------------
ViewMode Globals::viewMode = ViewMode::EXTERIOR;
namespace Globals {
  namespace InteriorView {
    float up[3] = {0, 1, 0};
    float rotH = 180;
    float rotV = 0;
    float eyePos[3] = {1, -1, 0};
    float lookDir[3] = {
         Sin(rotH)*Cos(rotV),
        -Sin(rotV),
        -Cos(rotH)*Cos(rotV)};
  }
}