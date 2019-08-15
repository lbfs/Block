#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

#define GameWindowWidth 640
#define GameWindowHeight 480

#define TileRenderSize 21
#define TileCountX 10
#define TileCountY 20

// I-Tetrimino // 0000 1111 0000 0000 | 0010 0010 0010 0010 | 0000 0000 1111 0000 | 0100 0100 0100 0100 
// J-Tetrimino // 1000 1110 0000 0000 | 0110 0100 0100 0000 | 0000 1110 0010 0000 | 0100 0100 1100 0000
// L-Tetrimino // 0010 1110 0000 0000 | 0100 0100 0110 0000 | 0000 1110 1000 0000 | 1100 0100 0100 0000
// O-Tetrimino // 0110 0110 0000 0000 | Repeat 3 times
// S-Tetrimino // 0110 1100 0000 0000 | 0100 0110 0010 0000 | 0000 0110 1100 0000 | 1000 1100 0100 0000
// T-Tetrimino // 0100 1110 0000 0000 | 0100 0110 0100 0000 | 0000 1110 0100 0000 | 0100 1100 0100 0000
// Z-Tetrimino // 1100 0110 0000 0000 | 0010 0110 0100 0000 | 0000 1100 0110 0000 | 0100 1100 1000 0000

/*
enum BlockType : uint16_t {};
enum BlockRotation : uint8_t { Top = 0, Left = 1, Bottom = 2, Right = 3 };
enum BlockColor : uint32_t { LightBlue = 0xFF00F0EF, DarkBlue = 0x0000EE, Orange = 0xFFEF9F00, Yellow = 0xFFEFF000, Green = 0xFF00EF00, Purple = 0xFF9F00F0, Red = 0xFF9F00F0 };

struct Block
{
	BlockType Type;
	BlockRotation Rotation;
	BlockColor Color;
	uint32_t GridX;
	uint32_t GridY;
};
*/

struct GraphicsInfo
{
	void* BitmapMemory;
	uint32_t Width;
	uint32_t Height;
};

void GameInitialize(GraphicsInfo* GameGraphicsInfo);
void GameUpdate(GraphicsInfo* GameGraphicsInfo);

#endif