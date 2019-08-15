#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

#define GameWindowWidth 640
#define GameWindowHeight 480

#define TileRenderSize 21
#define TileCountX 10
#define TileCountY 20
#define TileBorderColor 0xFF212121

const uint16_t BlockTypes[28] = {  3840,  8738,   240, 17476,   // I-Block 0
								  36352, 25664,  3616, 17600,   // J-Block 4
								  11776, 17504,  3712, 50240,   // L-Block 8
								  26112, 26112, 26112, 26112,   // O-Block 12
								  27648, 17952,  1728, 35904,   // S-Block 16
								  19968, 17984,  3648, 19520,   // T-Block 20
								  50688,  9792,  3168, 19584 }; // Z-Block 24

const uint32_t BlockColors[7] = { 0xFF00FFFF , 0xFF0000FF , 0xFFFFAA00 , 0xFFFFFF00 , 0xFF00FF00, 0xFF9900FF, 0xFFFF0000 };

struct Block
{
	uint16_t Type;
	uint16_t Rotation;
	uint32_t Color;
	int32_t GridX;
	int32_t GridY;
};


struct GameState
{
	uint32_t Grid[TileCountX][TileCountY];
	Block CurrentBlock;
	uint32_t RandomNumber;
};

struct GraphicsInfo
{
	void* BitmapMemory;
	uint32_t Width;
	uint32_t Height;
};

void MoveBlock(GameState* GameState, int32_t x, int32_t y);
void RapidDropBlock(GameState* GameStatus);
void RotateBlock(GameState* GameStatus);

void GameInitialize(GraphicsInfo* GameGraphicsInfo, GameState* GameState);
void GameUpdate(GraphicsInfo* GameGraphicsInfo, GameState* GameState);


#endif