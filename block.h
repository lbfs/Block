#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define GameWindowWidth 800
#define GameWindowHeight 600

#define TileRenderSize 26
#define TileRowCount 22
#define TileColumnCount 10
#define TileBorderColor 0xFF212121
#define TileBackgroundColor 0x00000000

#define AutomaticBlockDropTimeSeconds 0.5f

#define BoardStartPositionX 272
#define BoardStartPositionY 15

#define BlockRotationCount 4
#define BlockTypeCount 7

#define PreviewStartPositionX 73
#define PreviewStartPositionY 215

const uint16_t BlockTypes[28] = { 3840,  8738,   240, 17476,    // I-Block 0
								  36352, 25664,  3616, 17600,   // J-Block 4
								  11776, 17504,  3712, 50240,   // L-Block 8
								  26112, 26112, 26112, 26112,   // O-Block 12
								  27648, 17952,  1728, 35904,   // S-Block 16
								  19968, 17984,  3648, 19520,   // T-Block 20
								  50688,  9792,  3168, 19584 }; // Z-Block 24

const uint32_t BlockColors[7] = { 0xFF00FFFF , 0xFF0000FF , 0xFFFFAA00 , 0xFFFFFF00 , 0xFF00FF00, 0xFF9900FF, 0xFFFF0000 };

struct Block
{
	uint16_t Structure;
	uint16_t Type;
	uint16_t Rotation;
	uint32_t Color;
	int32_t Row;
	int32_t Column;
};

struct GraphicsInfo
{
	void* BitmapMemory;
	uint32_t Width;
	uint32_t Height;
};

struct GameState
{
	uint32_t Board[TileRowCount][TileColumnCount];
	Block CurrentBlock;
	Block NextBlock;
	uint32_t Score;
	uint32_t Level;
	clock_t Time;
	bool Playing;
};

void RotateBlock(GameState* GameStatus);
bool MoveBlock(GameState* GameStatus, int16_t Row, int16_t Column);
void PressBlock(GameState* GameStatus);
void DropBlock(GameState* GameStatus);

void GameInitialize(GraphicsInfo* GameGraphicsInfo, GameState* GameState);
void GameUpdate(GraphicsInfo* GameGraphicsInfo, GameState* GameState, char Key);

#endif