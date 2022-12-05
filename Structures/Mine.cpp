#include "Mine.hpp"
#include "globals.hpp"
#include "Objects.hpp"
#include "Shaders.hpp"

Mine::Mine()
{
	type = 5;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	// Padding between edge of chamber and edge of cell
	float padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	Create();
}

void Mine::Create()
{
	Chamber::Create();

	// FIXME this tethers OBJ to this particular Mine
	Objects::Mine->center = center;
	Objects::Mine->Create();
}

void Mine::ApplyNoise()
{
	Chamber::ApplyNoise();

	// FIXME this tethers OBJ to this particular Mine
	Objects::Mine->ApplyNoise();
}

void Mine::Draw(bool hasControl)
{
	Chamber::Draw();

	PushShader(Shader::threeDshader);
	glPushMatrix();
	glTranslatef(center.x, center.y, center.z);

	glFrontFace(GL_CW);
	Objects::Mine->Draw();
	glFrontFace(GL_CCW);

	glPopMatrix();
	PopShader();

	ErrCheck("Mine::Draw");
}