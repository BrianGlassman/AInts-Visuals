#include "Mine.hpp"
#include "globals.hpp"
#include "Shaders.hpp"

Mine::Mine()
{
	type = StructureType::Mine;
	sideType = SideType::chamber;
	noiseScale = Globals::chamberNoiseScale;

	// Padding between edge of chamber and edge of cell
	float padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	pieces.push_back(MinePiece(Objects::MineCore));
	// pieces.push_back(MinePiece(Objects::MineRightCap));
	// pieces.push_back(MinePiece(Objects::MineLeftCap));
	// pieces.push_back(MinePiece(Objects::MineTopCap));
	// pieces.push_back(MinePiece(Objects::MineBottomCap));
	// pieces.push_back(MinePiece(Objects::MineFrontCap));
	// pieces.push_back(MinePiece(Objects::MineBackCap));

	Create();
}

void Mine::PreCreate()
{
	Chamber::PreCreate();
}

void Mine::Create()
{
	PreCreate();

	Chamber::Create(false);

	for (auto&& piece : pieces)
	{
		piece.Create();
	}

	PostCreate();

	ErrCheck("Mine::Create");
}

void Mine::ApplyNoise()
{
	Chamber::ApplyNoise();

	for (auto&& piece : pieces)
	{
		piece.ApplyNoise();
	}
}

void Mine::Draw(bool hasControl)
{
	// Chamber::Draw();

	for (auto&& piece : pieces)
	{
		piece.Draw();
	}

	ErrCheck("Mine::Draw");
}