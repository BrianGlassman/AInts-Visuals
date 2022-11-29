#include <type_traits>

#include "globals.hpp"

#include "CSCIx229.h"

//---------
// Toggles
//---------
bool Toggles::Light::lightOrbiting = true;
bool Toggles::Light::smooth = true;
bool Toggles::Noise::showPerturbed = true;
bool Toggles::Noise::showPVectors = false;
bool Toggles::Noise::showBase = false;
bool Toggles::showNormals = false;
bool Toggles::debug = false;
bool Toggles::showCenterlines = true;

//--------
// Values
//--------
float Globals::tunnelNoiseScale =  0.3;
float Globals::chamberNoiseScale = 0.3;

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

Scene Globals::sceneChoice;

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
    Vector3 up = {0, 1, 0};
    float rotH = 180;
    float rotV = 0;
    Vector3 eyePos = {1, -1, 0};
    Vector3 lookDir = {
         Sin(rotH)*Cos(rotV),
        -Sin(rotV),
        -Cos(rotH)*Cos(rotV)};
    Vector3 right = (lookDir.Cross(up)).Normalized();
  }
}