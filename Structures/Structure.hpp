#pragma once

#include "util.hpp" // Model class

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
};