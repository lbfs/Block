#include "block.h"

#if _DEBUG
#include <windows.h>
#endif

// TODO: Scoring System, Show Next Block, Properly Start Blocks at Correct Position (I.E. Not from "0,0" of the Block)
// But rather 0,0 of the first hit block.

void
DrawCoordinateBox(GraphicsInfo* GameGraphicsInfo, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY)
{
	uint32_t* Pixel = (uint32_t*)GameGraphicsInfo->BitmapMemory;
	Pixel += (StartX + GameGraphicsInfo->Width * StartY);
	for (int Y = EndY - StartY; Y > 0; --Y )
	{
		for (int X = EndX - StartX; X > 0; --X)
		{
			*(Pixel + X) = HexColor;
		}
		Pixel += GameGraphicsInfo->Width;
	}
}

void
DrawBoard(GraphicsInfo* GameGraphicsInfo, GameState* GameStatus)
{
	uint16_t Type = GameStatus->CurrentBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (uint32_t Row = 0; Row < TileRowCount; Row++)
	{
		for (uint32_t Column = 0; Column < TileColumnCount; Column++)
		{
			uint32_t StartX = Column * TileRenderSize;
			uint32_t StartY = Row * TileRenderSize;
			int32_t EndX = StartX + TileRenderSize - 1;
			int32_t EndY = StartY + TileRenderSize - 1;
			DrawCoordinateBox(GameGraphicsInfo, GameStatus->Board[Row][Column], StartX, StartY, EndX, EndY);
		}
	}
}

void
DrawBlock(GraphicsInfo* GameGraphicsInfo, GameState* GameStatus)
{
	uint16_t Type = GameStatus->CurrentBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (uint32_t Row = 0; Row < 4; Row++)
	{
		for (uint32_t Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				uint32_t StartX = (Column * TileRenderSize) + (GameStatus->CurrentBlock.Column * TileRenderSize);
				uint32_t StartY = (Row * TileRenderSize) + (GameStatus->CurrentBlock.Row * TileRenderSize);
				int32_t EndX = StartX + TileRenderSize - 1;
				int32_t EndY = StartY + TileRenderSize - 1;
				DrawCoordinateBox(GameGraphicsInfo, GameStatus->CurrentBlock.Color, StartX, StartY, EndX, EndY);
			}
			HighBit >>= 1;
		}
	}
}

bool
CheckBoardRow(GameState* GameStatus, uint16_t Row)
{
	for (uint16_t Column = 0; Column < TileColumnCount; Column++)
	{
		if (GameStatus->Board[Row][Column] == TileBackgroundColor)
		{
			return false;
		}
	}
	return true;
}

void
CheckBoardForFullRows(GameState* GameStatus)
{
	// Partially filled or empty row is 0
	// Full row is 1
	// bool CheckBoardState[TileRowCount] = {false};
	for (uint16_t i = 0; i < TileRowCount - 1; i++)
	{
		for (uint16_t j = 0; j < TileRowCount - i - 1; j++)
		{
			if (CheckBoardRow(GameStatus, j) < CheckBoardRow(GameStatus, j + 1))
			{
				for (uint16_t Column = 0; Column < TileColumnCount; Column++)
				{
					auto temp = GameStatus->Board[j][Column];
					GameStatus->Board[j][Column] = GameStatus->Board[j + 1][Column];
					GameStatus->Board[j + 1][Column] = temp;
				}
			}

		}
	}

	// iterate over board
	for (uint16_t Row = 0; Row < TileRowCount; Row++)
	{
		if (!CheckBoardRow(GameStatus, Row))
			break;

		for (uint16_t Column = 0; Column < TileColumnCount; Column++)
		{
			GameStatus->Board[Row][Column] = TileBackgroundColor;
		}
	}
}

bool 
CanMoveBlock(GameState* GameStatus, Block CopyBlock)
{
	uint16_t Type = CopyBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (int Row = 0; Row < 4; Row++)
	{
		for (int Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				int32_t RealRow = CopyBlock.Row + Row;
				int32_t RealColumn = CopyBlock.Column + Column;
				uint32_t CheckColor = CopyBlock.Color;
				uint32_t BoardColor = GameStatus->Board[RealRow][RealColumn];

				if (RealColumn > TileColumnCount - 1 || RealColumn < 0 || RealRow > TileRowCount - 1 || RealRow < 0)
				{
					return false;
				}
				if (BoardColor != TileBackgroundColor)
				{
					return false;
				}
			}
			HighBit >>= 1;
		}
	}
	return true;
}

Block
RequestRandomBlock()
{
	Block RandomBlock = {};
	uint16_t BlockIndex = (rand() % BlockTypeCount);
	RandomBlock.Rotation = rand() % BlockRotationCount;
	RandomBlock.Type = ( BlockIndex * BlockRotationCount) + RandomBlock.Rotation;
	RandomBlock.Structure = BlockTypes[RandomBlock.Type];
	RandomBlock.Color = BlockColors[BlockIndex];

	// Assume starting position
	RandomBlock.Row = 0;
	RandomBlock.Column = 3;

	return RandomBlock;
}

void 
PressBlock(GameState* GameStatus)
{
	uint16_t Type = GameStatus->CurrentBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (int Row = 0; Row < 4; Row++)
	{
		for (int Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				int32_t RealColumn = GameStatus->CurrentBlock.Column + Column;
				int32_t RealRow = GameStatus->CurrentBlock.Row + Row;
				GameStatus->Board[RealRow][RealColumn] = GameStatus->CurrentBlock.Color;
			}
			HighBit >>= 1;
		}
	}

	if (GameStatus->Playing) // Move this, only here because the timer is still active.
	{
		CheckBoardForFullRows(GameStatus);
		Block RandomBlock = RequestRandomBlock();
		if (CanMoveBlock(GameStatus, RandomBlock))
		{
			GameStatus->CurrentBlock = RandomBlock;
		}
		else
		{
#if _DEBUG
			OutputDebugString("Game Over!\n");

#endif
			GameStatus->Playing = false;
		}
	}
}

void 
RotateBlock(GameState* GameStatus)
{
	Block CopyBlock = GameStatus->CurrentBlock;
	CopyBlock.Rotation = (CopyBlock.Rotation + 1) % BlockRotationCount;
	CopyBlock.Type = (CopyBlock.Type - GameStatus->CurrentBlock.Rotation) + CopyBlock.Rotation;
	CopyBlock.Structure = BlockTypes[CopyBlock.Type];
	if (CanMoveBlock(GameStatus, CopyBlock))
	{
		GameStatus->CurrentBlock = CopyBlock;
	}
}

bool 
MoveBlock(GameState* GameStatus, int16_t Row, int16_t Column)
{
	Block CopyBlock = GameStatus->CurrentBlock;
	CopyBlock.Row += Row;
	CopyBlock.Column += Column;
	if (CanMoveBlock(GameStatus, CopyBlock))
	{
		GameStatus->CurrentBlock = CopyBlock;
		return true;
	}
	else
	{	
		if (Row == 1 && Column == 0)
		{
			PressBlock(GameStatus);
		}
		return false;
	}
}

void
DropBlock(GameState* GameStatus)
{
	while (MoveBlock(GameStatus, 1, 0)) {}
}

void 
GameInitialize(GraphicsInfo* GameGraphicsInfo, GameState* GameStatus)
{
	// Seed the random block generator
	srand(time(NULL));

	// Wipe the video buffer
	uint32_t* Pixel = (uint32_t *)GameGraphicsInfo->BitmapMemory;
	for (uint32_t Row = 0; Row < GameGraphicsInfo->Height; Row++)
	{
		for (uint32_t Column = 0; Column < GameGraphicsInfo->Width; Column++) 
		{
			*Pixel++ = 0xFF313131;
		}
	}

	// Reset the board.
	for (uint32_t Row = 0; Row < TileRowCount; Row++)
	{
		for (uint32_t Column = 0; Column < TileColumnCount; Column++)
		{
			GameStatus->Board[Row][Column] = TileBackgroundColor;
		}
	}

	GameStatus->CurrentBlock = RequestRandomBlock();

	// Draw the board.
	DrawBoard(GameGraphicsInfo, GameStatus);

	// Start The Game
	GameStatus->Playing = true;
}

void 
GameUpdate(GraphicsInfo* GameGraphicsInfo, GameState* GameState)
{
	DrawBoard(GameGraphicsInfo, GameState);
	DrawBlock(GameGraphicsInfo, GameState);
}
