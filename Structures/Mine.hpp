#pragma once

#include "Chamber.hpp"

class Mine : public Chamber
{
public:
    Mine() : Mine(63) {}
    // FIXME temporary convenience until auto-generating is implemented
    Mine(unsigned char sides);

    void CreateInternal(int i0, bool f0, int i1, bool f1, int i2, bool f2);
    virtual void Create();
};