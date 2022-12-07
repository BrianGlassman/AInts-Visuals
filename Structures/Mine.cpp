#include "Mine.hpp"
#include "globals.hpp"
#include "Shaders.hpp"

Mine::Mine()
{
	type = StructureType::Mine;
	sideType = SideType::chamber;

	// Padding between edge of chamber and edge of cell
	float padding = 0.1;
	radius = 0.5 - padding;
	padScale = 1 - 2*padding;

	panelWidth = 1.0*padScale / panels;

	pieces.insert({Vector3Int::Zero, MinePiece(Objects::MineCore)});
	pieces.insert({Vector3Int::Right, MinePiece(Objects::MineRightCap)});
	pieces.insert({Vector3Int::Left, MinePiece(Objects::MineLeftCap)});
	pieces.insert({Vector3Int::Up, MinePiece(Objects::MineTopCap)});
	pieces.insert({Vector3Int::Down, MinePiece(Objects::MineBottomCap)});
	pieces.insert({Vector3Int::Forward, MinePiece(Objects::MineFrontCap)});
	pieces.insert({Vector3Int::Backward, MinePiece(Objects::MineBackCap)});

	Create();
}

void Mine::PreCreate()
{
	Chamber::PreCreate();
}

void Mine::Create()
{
	PreCreate();

	Chamber::Create(false, true, true);

	for (auto&& dirPiece : pieces)
	{
		auto& dir = dirPiece.first;
		auto& piece = dirPiece.second;
		if (dir == Vector3Int::Zero)
		{ // Always use core, don't join with chamber geometry
			piece.center = center;
			piece.Create(true);
		}
		else if (! GetSide(dir))
		{ // Only use caps if that side is unconnected, don't join with Chamber geometry
			piece.center = center;
			piece.Create(false);
		}
		else
		{ // If a piece was created before but isn't re-created, mark it old
			piece.created = false;
		}
	}

	PostCreate();

	ErrCheck("Mine::Create");
}

void Mine::ApplyNoise()
{
	Chamber::ApplyNoise();

	for (auto&& dirPiece : pieces)
	{
		auto& piece = dirPiece.second;
		if (piece.created) piece.ApplyNoise();
	}
}

void Mine::Draw(bool hasControl)
{
	Chamber::Draw(hasControl);

	if (hasControl) PushShader(Shader::threeDshader);
	// Only draw the inside from interior view
	if (Globals::viewMode == ViewMode::INTERIOR)
	{
		for (auto&& dirPiece : pieces)
		{
			auto& piece = dirPiece.second;
			if (piece.created) piece.Draw(false);
		}
	}
	if (hasControl) PopShader();

	ErrCheck("Mine::Draw");
}