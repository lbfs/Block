#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

#define GameWindowWidth 640
#define GameWindowHeight 480

#define TileRenderSize 21
#define TileCountX 10
#define TileCountY 20
#define TileBorderColor 0xFF212121

enum BlockColor : uint32_t { LightBlue = 0xFF00F0EF, DarkBlue = 0x0000EE, Orange = 0xFFEF9F00, Yellow = 0xFFEFF000, Green = 0xFF00EF00, Purple = 0xFF9F00F0, Red = 0xFF9F00F0 };
enum BlockRotation { Top = 0, Right = 1, Bottom = 2, Left = 3 };


const uint16_t BlockTypes[28] = {  3840,  8738,   240, 17476,   // I-Tetrimino 0
								  36352, 25664,  3616, 17600,   // J-Tetrimino 4
								  11776, 17504,  3712, 50240,   // L-Tetrimino 8
								  26112, 26112, 26112, 26112,   // O-Tetrimino 12
								  27648, 17952,  1728, 35904,   // S-Tetrimino 16
								  19968, 17984,  3648, 19520,   // T-Tetrimino 20
								  50688,  9792,  3168, 19584 }; // Z-Tetrimino 24

struct Block
{
	uint16_t Type;
	uint16_t Rotation;
	BlockColor Color;
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
void DropBlock(GameState* GameStatus);
void RotateBlock(GameState* GameStatus, BlockRotation Rotation);

void GameInitialize(GraphicsInfo* GameGraphicsInfo, GameState* GameState);
void GameUpdate(GraphicsInfo* GameGraphicsInfo, GameState* GameState);


#endif