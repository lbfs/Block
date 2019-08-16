#include "block.h"

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
	for (uint32_t Row = 0; Row < TileRowCount; Row++)
	{
		for (uint32_t Column = 0; Column < TileColumnCount; Column++)
		{
			uint32_t StartX = Column * TileRenderSize + BoardStartPositionX;
			uint32_t StartY = Row * TileRenderSize + BoardStartPositionY;
			int32_t EndX = StartX + TileRenderSize - 1;
			int32_t EndY = StartY + TileRenderSize - 1;
			DrawCoordinateBox(GameGraphicsInfo, GameStatus->Board[Row][Column], StartX, StartY, EndX, EndY);
		}
	}
}

void
DrawPreviewBoard(GraphicsInfo* GameGraphicsInfo)
{
	for (uint32_t Row = 0; Row < PreviewRowCount; Row++)
	{
		for (uint32_t Column = 0; Column < PreviewColumnCount; Column++)
		{
			uint32_t StartX = Column * TileRenderSize + PreviewStartPositionX;
			uint32_t StartY = Row * TileRenderSize + PreviewStartPositionY;
			int32_t EndX = StartX + TileRenderSize - 1;
			int32_t EndY = StartY + TileRenderSize - 1;
			DrawCoordinateBox(GameGraphicsInfo, TileBackgroundColor, StartX, StartY, EndX, EndY);
		}
	}
}

void
DrawBlock(GraphicsInfo* GameGraphicsInfo, Block CopyBlock, uint32_t BoardStartX, uint32_t BoardStartY)
{
	uint16_t Type = CopyBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (uint32_t Row = 0; Row < 4; Row++)
	{
		for (uint32_t Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				uint32_t StartX = (Column * TileRenderSize) + (CopyBlock.Column * TileRenderSize) + BoardStartX;
				uint32_t StartY = (Row * TileRenderSize) + (CopyBlock.Row * TileRenderSize) + BoardStartY;
				int32_t EndX = StartX + TileRenderSize - 1;
				int32_t EndY = StartY + TileRenderSize - 1;
				DrawCoordinateBox(GameGraphicsInfo, CopyBlock.Color, StartX, StartY, EndX, EndY);
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

// break into two functions CheckBoard and SortBoard

uint16_t
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

	uint16_t Count = 0;
	// iterate over board
	for (uint16_t Row = 0; Row < TileRowCount; Row++)
	{
		if (!CheckBoardRow(GameStatus, Row))
			break;

		for (uint16_t Column = 0; Column < TileColumnCount; Column++)
		{
			GameStatus->Board[Row][Column] = TileBackgroundColor;
		}

		Count++;
	}
	return Count;
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

	return RandomBlock;
}

void
UpdateScore(GameState * GameStatus, uint16_t RowsRemoved)
{ 
	switch (RowsRemoved)
	{
	case 1:
		GameStatus->Score += (40 * (GameStatus->Level + 1));
		break;
	case 2:
		GameStatus->Score += (100 * (GameStatus->Level + 1));
		break;
	case 3:
		GameStatus->Score += (300 * (GameStatus->Level + 1));
		break;
	case 4:
		GameStatus->Score += (1200 * (GameStatus->Level + 1));
		break;
	default: // Shouldnt be possible for > 0, max rows removed can only be 4
		break;
	}
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

	//Move all of this out

	// Update Gamescore
	uint16_t RowsRemoved = CheckBoardForFullRows(GameStatus);
	if (RowsRemoved)
	{
		UpdateScore(GameStatus, RowsRemoved);
	}
	// Assume starting position
	GameStatus->NextBlock.Row = 0;
	GameStatus->NextBlock.Column = 3;
	if (CanMoveBlock(GameStatus, GameStatus->NextBlock))
	{
		GameStatus->CurrentBlock = GameStatus->NextBlock;
	}
	else
	{
		GameStatus->State = Finished;
	}
	GameStatus->NextBlock = RequestRandomBlock();
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
	// TODO: move all of this out, only return if the block can be moved
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
ProcessKeyAction(GameState * GameStatus, char Key)
{
	// TODO: Remove all of this, standardize keys, put in main game loop with Switch Statement
	if (Key == 'S')
	{
		MoveBlock(GameStatus, 1, 0);
	}
	else if (Key == 'A')
	{
		MoveBlock(GameStatus, 0, -1);
	}
	else if (Key == 'D')
	{
		MoveBlock(GameStatus, 0, 1);
	}
	else if (Key == 'J')
	{
		RotateBlock(GameStatus);
	}
	else if (Key == 0x20)
	{
		DropBlock(GameStatus);
	}
}

void 
GameInitialize(GraphicsInfo* GameGraphicsInfo, GameState* GameStatus)
{
	// Seed the random block generator
	srand(time(NULL));

	// Wipe the video buffer
	uint32_t* Pixel = (uint32_t*)GameGraphicsInfo->BitmapMemory;
	for (uint32_t Row = 0; Row < GameGraphicsInfo->Height; Row++)
	{
		for (uint32_t Column = 0; Column < GameGraphicsInfo->Width; Column++)
		{
			*Pixel++ = 0xFF121212;
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
	// Assume starting position NOTE: REMOVE THIS AND THE ONE IN PRESSBLOCK();
	GameStatus->CurrentBlock.Row = 0;
	GameStatus->CurrentBlock.Column = 3;
	GameStatus->NextBlock = RequestRandomBlock();

	// Draw the board outline
	DrawCoordinateBox(GameGraphicsInfo, 0xFF9E351A, BoardStartPositionX - 1, BoardStartPositionY - 1, BoardStartPositionX + (TileRenderSize * TileColumnCount), BoardStartPositionY + (TileRenderSize * TileRowCount));

	// Draw the tile border
	DrawCoordinateBox(GameGraphicsInfo, 0xFF212121, BoardStartPositionX, BoardStartPositionY, BoardStartPositionX + (TileRenderSize * TileColumnCount) - 1, BoardStartPositionY + (TileRenderSize * TileRowCount) - 1);

	// Draw the preview board outline
	DrawCoordinateBox(GameGraphicsInfo, 0xFF9E351A, PreviewStartPositionX - 1, PreviewStartPositionY - 1, PreviewStartPositionX + (TileRenderSize * PreviewColumnCount), PreviewStartPositionY + (TileRenderSize * PreviewRowCount));

	// Draw the preview tile border
	DrawCoordinateBox(GameGraphicsInfo, 0xFF212121, PreviewStartPositionX, PreviewStartPositionY, PreviewStartPositionX + (TileRenderSize * PreviewColumnCount) - 1, PreviewStartPositionY + (TileRenderSize * PreviewRowCount) - 1);

	// Draw the board.
	DrawBoard(GameGraphicsInfo, GameStatus);

	GameStatus->State = Initalized;
}

void 
GameStart(GraphicsInfo* GameGraphicsInfo, GameState * GameStatus)
{
	if (GameStatus->State == Finished)
	{
		GameInitialize(GameGraphicsInfo, GameStatus);
	}
	if (GameStatus->State == Initalized)
	{
		GameStatus->State = Playing;
		GameStatus->Time = clock();
	}
}

void 
GameUpdate(GraphicsInfo* GameGraphicsInfo, GameState* GameState, char Key)
{
	if (GameState->State == Playing)
	{
		if ((clock() - GameState->Time) / (float)CLOCKS_PER_SEC > AutomaticBlockDropTimeSeconds)
		{
			ProcessKeyAction(GameState, 'S');
			GameState->Time = clock();
		}

		ProcessKeyAction(GameState, Key);
		DrawBoard(GameGraphicsInfo, GameState);
		DrawBlock(GameGraphicsInfo, GameState->CurrentBlock, BoardStartPositionX, BoardStartPositionY);

		DrawPreviewBoard(GameGraphicsInfo);
		DrawBlock(GameGraphicsInfo, GameState->NextBlock, PreviewStartPositionX + (TileRenderSize * PreviewColumnOffset), PreviewStartPositionY + (TileRenderSize * PreviewRowOffset)); // Draw Preview Block
	}
}
