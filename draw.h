#ifndef DRAW_H
#define DRAW_H

#include "block.h"

void DrawCoordinateBox(GameGraphics* Graphics, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY);
void DrawBlock(GameGraphics* Graphics, GameBlock Block, GameBoard* Board, bool UseBlockCoordinates);
void DrawBoard(GameGraphics* Graphics, GameBoard* Board, bool UseDefaultColor);

// void DrawSymbol(char ?);
// void DrawPhrase(string ?);
// void DrawNumber(uint16_t Number, PaddingZeros 0);

#endif