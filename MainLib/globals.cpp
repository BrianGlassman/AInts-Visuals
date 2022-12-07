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
bool Toggles::Noise::bezierFade = true;
bool Toggles::Display::showAxes = false;
bool Toggles::Display::wireframe = false;
bool Toggles::Display::faceCulling = true;
bool Toggles::showNormals = false;
bool Toggles::debug = false;
bool Toggles::showCenterlines = true;
bool Toggles::printMove = false;

//--------
// Values
//--------
float Globals::tunnelNoiseScale =  0.3;
float Globals::chamberNoiseScale = 0.3;
StructureType Globals::toBuild = StructureType::NONE;
SetColorMode Globals::setColorMode = SetColorMode::both;
// Special behavior for ++SetColorMode
SetColorMode& operator++(SetColorMode &val ) {
  using IntType = typename std::underlying_type<SetColorMode>::type;
  val = static_cast<SetColorMode>( static_cast<IntType>(val) + 1 );
  if (val == SetColorMode::END_OF_LIST )
    val = static_cast<SetColorMode>( static_cast<IntType>(SetColorMode::START_OF_LIST) + 1 );
  return val;
}
// Special behavior for SetColorMode++
SetColorMode operator++(SetColorMode &val, int ) {
  SetColorMode result = val;
  ++val;
  return result;
}
// Special behavior for --SetColorMode
SetColorMode& operator--(SetColorMode &val ) {
  using IntType = typename std::underlying_type<SetColorMode>::type;
  val = static_cast<SetColorMode>( static_cast<IntType>(val) - 1 );
  if (val == SetColorMode::START_OF_LIST )
    val = static_cast<SetColorMode>( static_cast<IntType>(SetColorMode::END_OF_LIST) - 1 );
  return val;
}
// Special behavior for SetColorMode--
SetColorMode operator--(SetColorMode &val, int ) {
  SetColorMode result = val;
  --val;
  return result;
}

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
std::unordered_map<Scene, _InteriorView> Globals::IVs({
  {Scene::colony, _InteriorView()},
  {Scene::tunnel, _InteriorView()},
  {Scene::chamber, _InteriorView()},
  {Scene::mine, _InteriorView()},
  {Scene::farm, _InteriorView()},
  {Scene::hill, _InteriorView()},
  {Scene::allTunnels, _InteriorView()},
  });
_InteriorView* Globals::InteriorView;