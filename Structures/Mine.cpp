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

	Chamber::Create(false);

	for (auto&& dirPiece : pieces)
	{
		auto& dir = dirPiece.first;
		if (dir == Vector3Int::Zero || GetSide(dir))
		{ // Always use core. Only use caps if that side exists
			auto& piece = dirPiece.second;
			piece.center = center;
			piece.Create();
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
		auto& dir = dirPiece.first;
		auto& piece = dirPiece.second;
		if (piece.created) piece.ApplyNoise();
	}
}

void Mine::Draw(bool hasControl)
{
	// Chamber::Draw();

	// Only draw the inside from interior view
	if (Globals::viewMode == ViewMode::INTERIOR)
	{
		PushShader(Shader::fixedPipeline);
		for (auto&& dirPiece : pieces)
		{
			auto& dir = dirPiece.first;
			auto& piece = dirPiece.second;
			if (piece.created) piece.Draw();
		}
	}

	ErrCheck("Mine::Draw");
}