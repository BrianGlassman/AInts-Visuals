#include "Structure.hpp"

void Structure::UnpackSides(unsigned char sides)
{
	forward = sides & 1;
	back = sides & 2;
	top = sides & 4;
	bottom = sides & 8;
	right = sides & 16;
	left = sides & 32;
}