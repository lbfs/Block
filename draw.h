#ifndef DRAW_H
#define DRAW_H

#include "block.h"

void DrawCoordinateBox(GameGraphics* GraphicsInfo, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY);
void DrawBlock(GameGraphics* Graphics, GameBlock Block, GameBoard* Board, bool UseBlockCoordinates);
void DrawBoard(GameGraphics* Graphics, GameBoard* Board, bool UseDefaultColor);

#endif