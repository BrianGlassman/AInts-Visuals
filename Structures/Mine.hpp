#pragma once

#include "Chamber.hpp"

class Mine : public Chamber
{
public:
    Mine();

    virtual void Draw(bool hasControl = true) override;
};