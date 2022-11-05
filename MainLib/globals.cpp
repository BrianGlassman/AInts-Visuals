#include <type_traits>

#include "globals.hpp"

//-------------------
// Input --> Display
//-------------------
float mouse_x = 0.5f;
float mouse_y = 0.5f;
float mouse_zoom = 1;

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


