#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define GameWindowWidth 800
#define GameWindowHeight 600
#define TileRenderSize 26
#define TileBorderColor 0xFF212121

#define TileRowCount 22
#define TileColumnCount 10
#define TileBackgroundColor 0x00000000

#define BoardStartPositionX 272
#define BoardStartPositionY 15

#define BlockRotationCount 4
#define BlockTypeCount 7

#define PreviewStartPositionX 73
#define PreviewStartPositionY 215
// TODO: improve block rendering, so we can have a small board square
// Maybe we should set a pivot bit and use that to determine the proper block coordinates?
#define PreviewRowCount 6
#define PreviewColumnCount 6

// Delayed Auto Shift
#define DASInitialDelayFrames 16
#define DASMinimumFrames 6

const uint16_t BlockTypes[28] = { 3840,  8738,   3840, 8738,    // I-Block 0
								  36352, 25664,  3616, 17600,   // J-Block 4
								  11776, 17504,  3712, 50240,   // L-Block 8
								  26112, 26112, 26112, 26112,   // O-Block 12
								  27648, 17952,  1728, 35904,   // S-Block 16
								  19968, 17984,  3648, 19520,   // T-Block 20
								  50688,  9792,  3168, 19584 }; // Z-Block 24

const uint32_t BlockColors[7] = { 0xFF00FFFF , 0xFF0000FF , 0xFFFFAA00 , 0xFFFFFF00 , 0xFF00FF00, 0xFF9900FF, 0xFFFF0000 };
const uint32_t GravitySpeeds[28] = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2};
// When >= 29, Assume 1

enum GameState { Finished = 0, Initalized = 1, Playing = 2};
enum GameKey { None, Left, Right, Down, Rotate, Drop };

struct GameBlock
{
	uint16_t Structure;
	uint16_t Type;
	uint16_t Rotation;
	uint32_t Color;
	int32_t X;
	int32_t Y;
};

struct GameBoard
{
	uint32_t ** Grid;
	uint32_t DefaultColor;
	uint16_t RowCount;
	uint16_t ColumnCount;
	uint16_t RenderSize;
	uint16_t RenderX;
	uint16_t RenderY;
	uint16_t RenderColumnOffset;
	uint16_t RenderRowOffset;
};

struct GameGraphics
{
	void* Buffer;
	uint32_t Width;
	uint32_t Height;
};

struct GameSession
{
	GameBoard Board;
	GameBoard PreviewBoard;
	GameBlock CurrentBlock;
	GameBlock NextBlock;
	uint32_t Score;
	uint32_t Level;
	uint32_t LinesCleared;
	GameState State;
	uint32_t CurrentFrameCount;
	uint32_t DropFrameCount;
	uint32_t DasCounter;
	GameKey PreviousKey;
};

GameBlock GetRandomBlock();
bool CheckBoardRow(GameBoard* Board, uint16_t Row);
void SortBoard(GameBoard* Board);
uint16_t ResetFullRows(GameBoard* Board);
void UpdateScore(GameSession* Session, uint16_t RowsRemoved);
bool CanMoveBlock(GameBoard* Board, GameBlock CopyBlock);
void PressBlock(GameBoard* Board, GameBlock CopyBlock);
GameBlock RotateBlock(GameBlock CopyBlock);
GameBlock MoveBlock(GameBlock CopyBlock, int16_t X, int16_t Y);
GameBlock DropBlock(GameSession* Session, GameBlock CopyBlock);
void ResetBoard(GameBoard* Board);
bool ProcessKeyAction(GameSession* Session, GameKey Key);
bool GameInitialize(GameGraphics* Graphics, GameSession* Session);
void GameStart(GameGraphics* Graphics, GameSession* Session);
void GameUpdate(GameGraphics* Graphics, GameSession* GameSession, GameKey Key);

#endif