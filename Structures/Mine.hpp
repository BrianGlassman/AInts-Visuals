#pragma once

#include "Chamber.hpp"

class Mine : public Chamber
{
public:
    Mine() : Chamber() {}
    Mine(unsigned char sides) : Chamber(sides) {}
    
    void ApplyNoise(float offset[]);
};