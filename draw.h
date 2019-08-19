#ifndef DRAW_H
#define DRAW_H

#include "block.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

union BitmapCharacterInfo
{
	uint16_t Elements[8];
	struct {
		uint16_t Id; // ASCII Table ID
		uint16_t X;
		uint16_t Y;
		uint16_t Width;
		uint16_t Height;
		uint16_t XOffset;
		uint16_t YOffset;
		uint16_t XAdvance;
	};
};

struct BitmapCharacters
{
	uint32_t Length;
	BitmapCharacterInfo* Elements;
};

void DrawCoordinateBox(GameGraphics* Graphics, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY);
void DrawBlock(GameGraphics* Graphics, GameBlock Block, GameBoard* Board, bool UseBlockCoordinates);
void DrawBoard(GameGraphics* Graphics, GameBoard* Board, bool UseDefaultColor);
BitmapCharacters* LoadFont(const char* Filename); //needs to be freed
void DrawWord(GameGraphics* Graphics, uint32_t StartX, uint32_t StartY);

#endif