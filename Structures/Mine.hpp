#pragma once

#include "Chamber.hpp"

class Mine : public Chamber
{
public:
    Mine();

    void Create() override;
    void ApplyNoise() override;
    void Draw(bool hasControl = true) override;
};