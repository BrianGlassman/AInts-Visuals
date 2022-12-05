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

void Mine::Draw(bool hasControl)
{
	Chamber::Draw();

	PushShader(Shader::threeDshader);

	Objects::Mine.Draw();

	PopShader();
}