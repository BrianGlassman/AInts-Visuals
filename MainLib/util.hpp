#include "CSCIx229.h"

#define ORIGIN 0.0, 0.0, 0.0
#define X_AXIS 1.0, 0.0, 0.0
#define Y_AXIS 0.0, 1.0, 0.0
#define Z_AXIS 0.0, 0.0, 1.0

class Cylinder
{
public:
    Cylinder();

    float center[3];
    bool top;
    bool sides;
    bool bottom;
    float radius;
    float height;
    int n;

    void Draw();
};