#include "Chamber.hpp"

void Chamber::Draw()
{
    static Cube cube;

	glPushMatrix(); {
		glTranslatef(center[0], center[1], center[2]);

        cube.Draw();
	} glPopMatrix();
}