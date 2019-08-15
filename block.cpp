#include "block.h"

void
DrawCoordinateBox(GraphicsInfo* GameGraphicsInfo, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY)
{
	uint32_t* Pixel = (uint32_t*)GameGraphicsInfo->BitmapMemory;
	Pixel += (StartX + GameGraphicsInfo->Width * StartY);
	for (int Y = 0; Y <= EndY - StartY; ++Y)
	{
		for (int X = 0; X <= EndX - StartX; ++X)
		{
			*(Pixel + X) = HexColor;
		}
		Pixel += GameGraphicsInfo->Width;
	}
}

void
DrawGrid(GraphicsInfo * GameGraphicsInfo, uint32_t GridColor, int StartX, int StartY)
{
	uint32_t GridWidth = (StartX + (TileCountX * TileRenderSize)) - StartX;
	uint32_t GridHeight = (StartY + (TileCountY * TileRenderSize)) - StartY;

	uint32_t* Row = (uint32_t*)GameGraphicsInfo->BitmapMemory;
	Row += (StartX + GameGraphicsInfo->Width * StartY);

	for (uint32_t RowIndex = 0; RowIndex < GridHeight; RowIndex++)
	{
		uint32_t* Pixel = Row;
		for (uint32_t ColumnIndex = 0; ColumnIndex < GridWidth; ColumnIndex++)
		{
			if ((RowIndex % TileRenderSize == 0 && RowIndex != 0) || (ColumnIndex % TileRenderSize == 0 && ColumnIndex != 0))
			{
				*Pixel = GridColor;
			}
			Pixel++;
		}
		Row += GameGraphicsInfo->Width;
	}
}

void
DrawTetrisGrid(GraphicsInfo * GameGraphicsInfo, GameState * GameStatus, int32_t StartX, int32_t StartY)
{
	for (int GridX = 0; GridX < TileCountX; GridX++)
	{
		for (int GridY = 0; GridY < TileCountY; GridY++)
		{
			int32_t ElementStartX = StartX + (GridX * TileRenderSize);
			int32_t ElementStartY = StartY + (GridY * TileRenderSize);
			int32_t ElementEndX = ElementStartX + TileRenderSize - 1;
			int32_t ElementEndY = ElementStartY + TileRenderSize - 1;
			DrawCoordinateBox(GameGraphicsInfo, GameStatus->Grid[GridX][GridY], ElementStartX, ElementStartY, ElementEndX, ElementEndY);
		}
	}
	DrawGrid(GameGraphicsInfo, TileBorderColor, StartX, StartY);
}

void
DrawTetrisBlock(GraphicsInfo * GameGraphicsInfo, Block BlockInfo)
{
	uint16_t bits = BlockTypes[BlockInfo.Type];
	uint16_t hbit = 1 << (16 - 1);
	for (int BlockX = 0; BlockX < 4; BlockX++)
	{
		for (int BlockY = 0; BlockY < 4; BlockY++)
		{
			if (bits & hbit)
			{
				int32_t ElementStartX = (BlockInfo.GridX * TileRenderSize) + (BlockY * TileRenderSize) + 216; // possible bug? // move 216, 31 elsewhere
				int32_t ElementStartY = (BlockInfo.GridY * TileRenderSize) + (BlockX * TileRenderSize) + 31;
				int32_t ElementEndX = ElementStartX + TileRenderSize - 1;
				int32_t ElementEndY = ElementStartY + TileRenderSize - 1;
				DrawCoordinateBox(GameGraphicsInfo, BlockInfo.Color, ElementStartX, ElementStartY, ElementEndX, ElementEndY);
			}
			hbit >>= 1;
		}
	}
}

Block
GetRandomBlock(GameState* GameStatus)
{
	Block TestBlock = {};
	TestBlock.Color = Green;
	TestBlock.Type = 4;
	TestBlock.GridX = 0;
	TestBlock.GridY = 0;
	return TestBlock;
}

bool
BlockCanMove(GameState* GameStatus, Block BlockInfo)
{
	uint16_t bits = BlockTypes[BlockInfo.Type];
	uint16_t hbit = 1 << (16 - 1);
	for (int BlockX = 0; BlockX < 4; BlockX++)
	{
		for (int BlockY = 0; BlockY < 4; BlockY++)
		{
			if (bits & hbit)
			{
				int32_t RealX = BlockInfo.GridX + BlockY;
				int32_t RealY = BlockInfo.GridY + BlockX;
				if (RealX >= TileCountX || RealX < 0 || RealY >= TileCountY || RealY < 0)
				{
					return false;
				}
				if (GameStatus->Grid[RealX][RealY] != 0xFF000000)
				{
					return false;
				}
			}
			hbit >>= 1;
		}
	}
	return true;
}

//add error checking
void 
DropBlock(GameState* GameStatus) 
{ 
	uint16_t bits = BlockTypes[GameStatus->CurrentBlock.Type];
	uint16_t hbit = 1 << (16 - 1);
	for (int BlockX = 0; BlockX < 4; BlockX++)
	{
		for (int BlockY = 0; BlockY < 4; BlockY++)
		{
			if (bits & hbit)
			{
				int32_t RealX = GameStatus->CurrentBlock.GridX + BlockY;
				int32_t RealY = GameStatus->CurrentBlock.GridY + BlockX;
				GameStatus->Grid[RealX][RealY] = GameStatus->CurrentBlock.Color;
			}
			hbit >>= 1;
		}
	}

	GameStatus->CurrentBlock = GetRandomBlock(GameStatus);
}

void
RotateBlock(GameState* GameStatus, BlockRotation Rotation)
{
	Block TestBlock = GameStatus->CurrentBlock;
	uint16_t NextRotation = (uint16_t)Rotation;
	TestBlock.Type = ((uint16_t)GameStatus->CurrentBlock.Type - (uint16_t)GameStatus->CurrentBlock.Rotation) + NextRotation;
	TestBlock.Rotation = (BlockRotation)NextRotation;
	if (BlockCanMove(GameStatus, TestBlock))
	{
		GameStatus->CurrentBlock = TestBlock;
	}
}

void
MoveBlock(GameState* GameStatus, int32_t x, int32_t y)
{
	Block TestBlock = GameStatus->CurrentBlock;
	TestBlock.GridX += x;
	TestBlock.GridY += y;
	if (BlockCanMove(GameStatus, TestBlock))
	{
		GameStatus->CurrentBlock = TestBlock;
	}
}

void 
DrawEntireTetrisGrid(GraphicsInfo* GameGraphicsInfo, GameState * GameState)
{
	DrawCoordinateBox(GameGraphicsInfo, 0xFF9E351A, 215, 30, 426, 451); // Replace with box outline
	DrawCoordinateBox(GameGraphicsInfo, 0xFF000000, 216, 31, 425, 450);
	DrawTetrisGrid(GameGraphicsInfo, GameState, 216, 31);
}


void
GameInitialize(GraphicsInfo* GameGraphicsInfo, GameState* GameStatus)
{
	for (int GridX = 0; GridX < TileCountX; GridX++)
	{
		for (int GridY = 0; GridY < TileCountY; GridY++)
		{
			GameStatus->Grid[GridX][GridY] = 0xFF000000;
		}
	}

	DrawCoordinateBox(GameGraphicsInfo, 0xFF121212, 0, 0, GameGraphicsInfo->Width - 1, GameGraphicsInfo->Height - 1);
	DrawEntireTetrisGrid(GameGraphicsInfo, GameStatus);

	GameStatus->CurrentBlock = GetRandomBlock(GameStatus);
}

void
GameUpdate(GraphicsInfo* GameGraphicsInfo, GameState* GameState)
{
	DrawEntireTetrisGrid(GameGraphicsInfo, GameState);
	DrawTetrisBlock(GameGraphicsInfo, GameState->CurrentBlock);
}